#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   define MENGINE_ASSERTION_TYPE(Ptr, Type, ...) if(dynamic_cast<Type>(Ptr) == nullptr) Mengine::Helper::AssertionOperator( MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_FATAL, #Ptr " invalid type: '" #Type "'", MENGINE_CODE_FILE, MENGINE_CODE_LINE ) (__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_TYPE(Ptr, Type, ...)
#endif