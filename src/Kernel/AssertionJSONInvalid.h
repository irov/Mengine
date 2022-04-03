#pragma once

#include "Config/Config.h"

#if MENGINE_ASSERTION_DEBUG
#   include "Kernel/Assertion.h"
#   include "Kernel/JSON.h"

#   define MENGINE_ASSERTION_JSON_INVALID( json, ... ) if( json == jpp::detail::invalid ) Helper::AssertionOperator( ASSERTION_LEVEL_FATAL, #json " is invalid", MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )(__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_JSON_INVALID( json, ... )
#endif