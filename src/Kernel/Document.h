#pragma once

#include "Config/Config.h"

#ifdef MENGINE_DEBUG
#   include "Config/Char.h"
#   include <stdio.h>
#	define MENGINE_DOCUMENT_FUNCTION MENGINE_CODE_FUNCTION
#   define MENGINE_DOCUMENT(...) [=]() -> const Char * { static Char buffer[4096]; sprintf( buffer, __VA_ARGS__ ); return buffer;}()
#else
#	define MENGINE_DOCUMENT_FUNCTION ""
#   define MENGINE_DOCUMENT(...) ""
#endif