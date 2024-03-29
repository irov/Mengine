#pragma once

#include "Config/Utf8.h"
#include "Config/Utf32.h"
#include "Config/StdInt.h"

namespace Mengine
{
	namespace Helper
	{
        bool Utf8NextCode( const Utf8 ** _utf8, const Utf8 * _utf8End, uint32_t * const _utf8Code );
        bool Utf8Validate( const Utf8 * _utf8, const Utf8 * _utf8End, const Utf8 ** const _utf8Invalid );
        const Utf8 * Utf8ReplaceInvalid( const Utf8 * _utf8, const Utf8 * _utf8End, Utf8 * const _utf8Out );
        bool Utf32ToUtf8( Utf32 _utf32, Utf8 _utf8[5] );
    }
}