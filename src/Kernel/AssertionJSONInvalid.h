#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/JSON.h"

#   define MENGINE_ASSERTION_JSON_INVALID( json, ... ) if( json == jpp::detail::invalid ) Mengine::Helper::AssertionOperator( MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_FATAL, #json " is invalid", MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION ) DETAIL__MENGINE_ASSERTION_CALL(__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_JSON_INVALID( json, ... )
#endif