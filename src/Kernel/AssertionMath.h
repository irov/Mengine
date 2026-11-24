#pragma once

#include "Kernel/Assertion.h"

#include "Config/StdMath.h"

#define MENGINE_ASSERTION_ISINFINITE( Value, ... ) MENGINE_ASSERTION_FATAL( Mengine::StdMath::isfinite( (Value) ) == true, __VA_ARGS__ )
#define MENGINE_ASSERTION_NOT_NAN( Value, ... ) MENGINE_ASSERTION_FATAL( Mengine::StdMath::isnan( (Value) ) == false, __VA_ARGS__ )
#define MENGINE_ASSERTION_NOT_INF( Value, ... ) MENGINE_ASSERTION_FATAL( Mengine::StdMath::isinf( (Value) ) == false, __VA_ARGS__ )
