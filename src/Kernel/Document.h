#pragma once

#ifndef MENGINE_MASTER_RELEASE
#   include "Config/Char.h"
#   include <stdio.h>
#   define MENGINE_DOCUMENT(...) [=]() -> const Char * { static Char buffer[1024]; sprintf( buffer, __VA_ARGS__ ); return buffer;}()
#else
#   define MENGINE_DOCUMENT(...) ""
#endif