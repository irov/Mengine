#pragma once

#include "Assertion.h"

#define MENGINE_ASSERTION_NOT_IMPLEMENTED() Mengine::Assertion( "This method not implemented, don't use that", __FILE__, __LINE__ )