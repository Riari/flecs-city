#pragma once

#include <Module/Module.h>

#include "Macros.h"

#ifdef TEST_EXPORTS
#define TEST_API API_EXPORT
#else
#define TEST_API API_IMPORT
#endif

namespace fc::Test
{

TEST_API extern fc::Module MODULE;

}; // namespace fc::Test
