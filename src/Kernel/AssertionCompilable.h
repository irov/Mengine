#pragma once

#include "Kernel/Assertion.h"

#include "Kernel/Compilable.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   define MENGINE_ASSERTION_COMPILABLE( Compilable, ... ) if( (Compilable)->isCompile() == false ) Mengine::Helper::AssertionOperator( MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_FATAL, "Compilable::isCompile == false", MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION ) DETAIL__MENGINE_ASSERTION_CALL(__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_COMPILABLE( Compilable, ... )
#endif