#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   define MENGINE_ASSERTION_MEMORY_PANIC_SAFE( Memory, Ret, ... ) if( (Memory) == nullptr ) {Mengine::Helper::AssertionOperator( MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_ERROR, #Memory " == nullptr", MENGINE_CODE_FILE, MENGINE_CODE_LINE )(__VA_ARGS__); return (Ret);}
#   define MENGINE_ASSERTION_MEMORY_PANIC( Memory, ... ) if( (Memory) == nullptr ) Mengine::Helper::AssertionOperator( MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_FATAL, #Memory " == nullptr", MENGINE_CODE_FILE, MENGINE_CODE_LINE )(__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_MEMORY_PANIC_SAFE( Memory, Ret, ... )
#   define MENGINE_ASSERTION_MEMORY_PANIC( Memory, ... )
#endif