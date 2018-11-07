#pragma once

#include "Assertion.h"

#ifndef NDEBUG
#   define MENGINE_ASSERTION_NOT_IMPLEMENTED() Mengine::Assertion( "true", __FILE__, __LINE__, "This method not implemented, don't use that" )
#else
#   define MENGINE_ASSERTION_NOT_IMPLEMENTED()
#endif