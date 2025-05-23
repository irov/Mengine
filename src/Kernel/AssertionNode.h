#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/Node.h"

#   define MENGINE_ASSERTION_NODE(Node, Type, ...) if(Mengine::Helper::dynamicNodeCast<Type>(Node) == nullptr) Mengine::Helper::AssertionOperator( MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_FATAL, #Node " invalid type '" #Type "'", MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION ) DETAIL__MENGINE_ASSERTION_CALL(__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_NODE(Node, Type, ...)
#endif