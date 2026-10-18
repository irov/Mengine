#pragma once

#include "Kernel/String.h"
#include "Kernel/WString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool Win32Utf8ToUnicode( const String & _utf8, WString * const _unicode );
        bool Win32Utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WString * const _unicode );
        bool Win32UnicodeToUtf8( const WChar * _unicode, String * const _utf8 );
        bool Win32UnicodeToUtf8( const WChar * _unicode, Char * const _utf8, size_t _capacity );
        //////////////////////////////////////////////////////////////////////////
    }
}
