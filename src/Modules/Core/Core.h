#pragma once

#include <Module/Module.h>

#include "Macros.h"

#ifdef CORE_EXPORTS
#define CORE_API API_EXPORT
#else
#define CORE_API API_IMPORT
#endif

namespace fc::Core
{

CORE_API extern fc::Module MODULE;

}; // namespace fc::Core
