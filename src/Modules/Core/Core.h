#pragma once

#include <Module/Macros.h>
#include <Module/Module.h>

#ifdef CORE_EXPORTS
#define CORE_API API_EXPORT
#else
#define CORE_API API_IMPORT
#endif

namespace fc::Core
{

CORE_API extern fc::Module MODULE;

}; // namespace fc::Core
