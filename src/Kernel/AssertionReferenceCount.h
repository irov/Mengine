#pragma once

#include "Config/Config.h"

#ifdef MENGINE_ASSERTION_DEBUG_ENABLE
#   include "Kernel/Assertion.h"

#   define MENGINE_ASSERTION_REFERENCE_COUNT(Ref, ...) if(Ref.isReferencing() == true) Mengine::Helper::AssertionOperator( Mengine::ASSERTION_LEVEL_FATAL, #Ref " if referecing", MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE ) (__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_REFERENCE_COUNT(Ref, ...)
#endif