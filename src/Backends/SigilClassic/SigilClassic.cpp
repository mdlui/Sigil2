#include "SigilClassic.hpp"
#include "Sigil2/SigiLog.hpp"

namespace SigilClassic
{


SigilContext::SigilContext()
{
    setThreadContext(0);
    enterEntity("__BEGINNING_OF_SIGIL__");
}


auto SigilContext::setThreadContext(TID tid) -> void
{
    if(cur_tid != tid)
    {
        cur_tcxt = &thread_contexts[tid];

        cur_eid         = &cur_tcxt->cur_eid;
        cur_entity_ids  = &cur_tcxt->entity_ids;
        cur_entity_data = &cur_tcxt->entity_data;
        cur_callstack   = &cur_tcxt->callstack;
        cur_entity      = &(*cur_entity_data)[*cur_eid];

        cur_tid = tid;
    }
}


auto SigilContext::enterEntity(std::string name) -> void
{
    /* Initialize new metadata in map, and set name */

    /* count is not bounded, error if too many functions */
    if(__builtin_add_overflow(global_eid_cnt, 1, &global_eid_cnt))
        SigiLog::fatal("SigilClassic detected overflow in entity count");
    EID caller = *cur_eid;
    *cur_eid = global_eid_cnt;

    auto it = cur_entity_ids->emplace(name, *cur_eid);
    auto p  = cur_entity_data->emplace(*cur_eid, EntityData{});

    cur_entity         = &p.first->second;
    cur_entity->name   = &it->first;
    cur_entity->caller = caller;

    cur_callstack->push(*cur_eid);
}


auto SigilContext::exitEntity() -> void
{
    cur_callstack->pop();
    *cur_eid = cur_callstack->top();
}


auto SigilContext::monitorWrite(Addr addr, ByteCount bytes) -> void
{
    for(int i = 0; i < bytes; ++i)
    {
        auto cur_addr = addr + i;
        sm.updateWriter(cur_addr, 1, *cur_eid);
    }
}


auto SigilContext::monitorRead(Addr addr, ByteCount bytes) -> void
{
    for(int i = 0; i < bytes; ++i)
    {
        auto cur_addr = addr + i;
        auto writer = sm.getWriterFID(cur_addr);

        if/*local*/((writer == *cur_eid) ||
                    sm.isReaderFID(cur_addr, *cur_eid))
        {
            cur_entity->local_bytes_read++;
        }
        else/*unique*/
        {
            cur_entity->comm_edges[writer]++;
        }
    }
}


auto SigilContext::incrIOPCost() -> void
{
    cur_entity->iops++;
}


auto SigilContext::incrFLOPCost() -> void
{
    cur_entity->flops++;
}


}; //end namespace SigilClassic
