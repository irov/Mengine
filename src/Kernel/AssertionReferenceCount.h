#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   define MENGINE_ASSERTION_REFERENCE_COUNT(Ref, ...) if(Ref.isReferencing() == true) Mengine::Helper::AssertionOperator( MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_FATAL, #Ref " if referecing", MENGINE_CODE_FILE, MENGINE_CODE_LINE , MENGINE_CODE_FUNCTION) DETAIL__MENGINE_ASSERTION_CALL(__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_REFERENCE_COUNT(Ref, ...)
#endif