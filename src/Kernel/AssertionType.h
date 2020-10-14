#pragma once

#include "Kernel/Assertion.h"

//////////////////////////////////////////////////////////////////////////
#if MENGINE_ASSERTION_DEBUG
#   define MENGINE_ASSERTION_TYPE(Ptr, Type, ...) if(dynamic_cast<Type>(Ptr) == nullptr) Mengine::Helper::AssertionOperator( Mengine::ASSERTION_LEVEL_FATAL, #Ptr " invalid type: '" #Type "'", MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE ) (__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_TYPE(Ptr, Type, ...)
#endif
//////////////////////////////////////////////////////////////////////////
