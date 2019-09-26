#pragma once

#ifdef MENGINE_DEBUG
#   include "Kernel/BufferHelper.h"

#	define MENGINE_DOCUMENT_FUNCTION MENGINE_CODE_FUNCTION
#   define MENGINE_DOCUMENT(...) [=]() -> const Char * { static Char buffer[4096] = {0}; Mengine::Helper::bufferFormat( buffer, 4096, __VA_ARGS__ ); return buffer;}()
#else
#	define MENGINE_DOCUMENT_FUNCTION ""
#   define MENGINE_DOCUMENT(...) ""
#endif