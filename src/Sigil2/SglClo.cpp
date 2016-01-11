#include "SglClo.hpp"
#include "SglInit.hpp"

#include <getopt.h>
#include <cstring>

#include <iostream>
using std::cout;
using std::endl;

namespace sgl
{
	constexpr char const* synchrotrace = "stgen";

	static struct option long_options[] =
	{
		{"help",     no_argument,       0, 'h' },
		{"backend",  required_argument, 0, 'b' },
		{"frontend", required_argument, 0, 'f' },
		{0,          0,                 0,  0  }
	};

	void Clo::parse()
	{

		int c;
		int option_index = 0;
		while ( (c = getopt_long(argc,argv,"hb:f:", long_options, &option_index) ) != 0 )
		{
			if (c == -1)
			{
				break;
			}

			switch (c)
			{
				case 0:
					if (long_options[option_index].flag != 0)
						break;

				case 'h':
					break;
				case 'b':
					if ( strcmp(optarg,synchrotrace) == 0 )
					{
						setSynchroTrace();
					}
					break;
				case 'f':
					break;
			}
		}
	}
};
