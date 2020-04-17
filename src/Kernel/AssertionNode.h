#pragma once

#include "Kernel/Assertion.h"
#include "Kernel/Node.h"

//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_ASSERTION_DEBUG
#   define MENGINE_ASSERTION_NODE(Node, Type, ...) if(Helper::dynamicNodeCast<Type>(Node) == nullptr) Mengine::Helper::AssertionOperator( Mengine::ASSERTION_LEVEL_FATAL, #Node " invalid type '" #Type "'", MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE ) (__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_NODE(Node, Type, ...)
#endif
//////////////////////////////////////////////////////////////////////////
