#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/PathString.h"
#include "Kernel/String.h"
#include "Kernel/WString.h"

#include "Config/Char.h"
#include "Config/Utf8.h"
#include "Config/Utf32.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool utf8NextCode( const Utf8 ** _utf8, const Utf8 * _utf8End, uint32_t * const _utf8Code );
        bool utf8Validate( const Utf8 * _utf8, const Utf8 * _utf8End, const Utf8 ** const _utf8Invalid );
        const Utf8 * utf8ReplaceInvalid( const Utf8 * _utf8, const Utf8 * _utf8End, Utf8 * const _utf8Out );
        bool utf32ToUtf8( Utf32 _utf32, Utf8 _utf8[5] );
        //////////////////////////////////////////////////////////////////////////
        bool unicodeToUtf8( const WChar * _unicode, String * const _utf8 );
        bool unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, String * const _utf8 );
        bool unicodeToUtf8( const WString & _unicode, String * const _utf8 );
        bool unicodeToUtf8( const WChar * _unicode, Char * const _utf8, size_t _utf8Capacity, size_t * const _utf8Size = nullptr );
        bool unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * const _utf8, size_t _utf8Capacity, size_t * const _utf8Size = nullptr );
        bool unicodeToUtf8( const WString & _unicode, Char * const _utf8, size_t _utf8Capacity, size_t * const _utf8Size = nullptr );
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const Char * _utf8, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _unicodeSize = nullptr );
        bool utf8ToUnicode( const Char * _utf8, WString * const _unicode );
        bool utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _unicodeSize = nullptr );
        bool utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WString * const _unicode );
        bool utf8ToUnicode( const String & _utf8, WString * const _unicode );
        bool utf8ToUnicode( const String & _utf8, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _unicodeSize = nullptr );
        bool utf8ToUnicode( const ConstString & _utf8, WString * const _unicode );
        bool utf8ToUnicode( const ConstString & _utf8, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _unicodeSize = nullptr );
        bool utf8ToUnicode( const PathString & _utf8, WString * const _unicode );
        bool utf8ToUnicode( const PathString & _utf8, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _unicodeSize = nullptr );
        bool utf8ToUnicode( const FilePath & _utf8, WString * const _unicode );
        bool utf8ToUnicode( const FilePath & _utf8, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _unicodeSize = nullptr );
        //////////////////////////////////////////////////////////////////////////
    }
}