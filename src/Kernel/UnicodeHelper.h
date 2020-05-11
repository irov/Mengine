#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/PathString.h"
#include "Kernel/String.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool unicodeToUtf8Size( const WChar * _unicode, size_t _unicodeSize, String & _utf8 );
        bool unicodeToUtf8( const WString & _unicode, String & _utf8 );
        bool unicodeToUtf8( const WChar * _unicode, Char * _utf8, size_t _utf8Capacity, size_t * _utf8Size = nullptr );
        bool unicodeToUtf8Size( const WChar * _unicode, size_t _unicodeSize, Char * _utf8, size_t _utf8Capacity, size_t * _utf8Size = nullptr );
        bool unicodeToUtf8( const WString & _unicode, Char * _utf8, size_t _utf8Capacity, size_t * _utf8Size = nullptr );
        //////////////////////////////////////////////////////////////////////////
        bool utf8ToUnicode( const Char * _utf8, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize = nullptr );
        bool utf8ToUnicode( const Char * _utf8, WString & _unicode );
        bool utf8ToUnicodeSize( const Char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize = nullptr );
        bool utf8ToUnicodeSize( const Char * _utf8, size_t _utf8Size, WString & _unicode );
        bool utf8ToUnicode( const String & _utf8, WString & _unicode );
        bool utf8ToUnicode( const String & _utf8, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize = nullptr );
        bool utf8ToUnicode( const ConstString & _utf8, WString & _unicode );
        bool utf8ToUnicode( const ConstString & _utf8, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize = nullptr );
        bool utf8ToUnicode( const PathString & _utf8, WString & _unicode );
        bool utf8ToUnicode( const PathString & _utf8, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize = nullptr );
        bool utf8ToUnicode( const FilePath & _utf8, WString & _unicode );
        bool utf8ToUnicode( const FilePath & _utf8, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize = nullptr );
    }
}