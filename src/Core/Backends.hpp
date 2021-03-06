#ifndef SIGIL2_BACKEND_H
#define SIGIL2_BACKEND_H

#include "Primitive.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <map>

class BackendIface
{
  public:
    virtual ~BackendIface() {}
    virtual auto onMemEv(const sigil2::MemEvent &) -> void {}
    virtual auto onCompEv(const sigil2::CompEvent &) -> void {}
    virtual auto onSyncEv(const sigil2::SyncEvent &) -> void {}
    virtual auto onCxtEv(const sigil2::CxtEvent &) -> void {}
    virtual auto onCFEv(const SglCFEv &) -> void {}
};

using ToolName = std::string;
using Args = std::vector<std::string>;
using BackendPtr = std::unique_ptr<BackendIface>;
using BackendIfaceGenerator = std::function<BackendPtr(void)>;

using BackendParser = std::function<void(const Args &)>;
/* Args passed from the command line to the backend */

using BackendFinish = std::function<void(void)>;
/* Invoked one time once all events have been passed to the backend */

struct Backend
{
    BackendIfaceGenerator generator;
    BackendParser parser;
    BackendFinish finish;
    sigil2::capabilities caps;
    Args args;
};


class BackendFactory
{
  public:
    BackendFactory()  = default;
    ~BackendFactory() = default;

    auto create(ToolName name, Args args) const -> Backend;
    auto add(ToolName name, Backend be) -> void;
    auto exists(ToolName name) const -> bool;
    auto available() const -> std::vector<std::string>;

  private:
    std::map<ToolName, Backend> registry;
};

#endif
