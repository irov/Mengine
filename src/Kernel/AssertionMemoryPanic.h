#pragma once

#include "Config/Config.h"

#if MENGINE_ASSERTION_DEBUG
#   include "Kernel/Assertion.h"

#   define MENGINE_ASSERTION_MEMORY_PANIC( memory, ... ) if( (memory) == nullptr ) Mengine::Helper::AssertionOperator( Mengine::ASSERTION_LEVEL_FATAL, #memory " == nullptr", MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )(__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_MEMORY_PANIC( memory, ... )
#endif