#pragma once

#include "Kernel/Assertion.h"

#ifdef MENGINE_DEBUG
#   define MENGINE_ASSERTION_MEMORY_PANIC( memory, Ret ) if( memory == nullptr ) return Helper::makeAssertionReturnOperator(Ret) << Helper::AssertionOperator( ASSERTION_LEVEL_FATAL, "memory == nullptr", __FILE__, __LINE__ )
#   define MENGINE_ASSERTION_MEMORY_PANIC_VOID( memory ) if( memory == nullptr ) return Helper::makeAssertionReturnOperator() << Helper::AssertionOperator( ASSERTION_LEVEL_FATAL, "memory == nullptr", __FILE__, __LINE__ )
#else
#   define MENGINE_ASSERTION_MEMORY_PANIC( memory, Ret )
#   define MENGINE_ASSERTION_MEMORY_PANIC_VOID( memory )
#endif