#pragma once

#include "Config/Char.h"

#include <cstring>
#include <cwchar>

namespace Mengine
{
    namespace StdString
    {
        using std::strlen;
        using std::strcmp;
        using std::strncmp;
        using std::strcpy;
        using std::strncpy;
        using std::strcat;
        using std::strncat;
        using std::strchr;
        using std::strrchr;
        using std::strstr;

        using std::wcslen;
        using std::wcscmp;
        using std::wcsncmp;
        using std::wcscpy;
        using std::wcsncpy;
        using std::wcscat;
        using std::wcsncat;
        using std::wcschr;
        using std::wcsrchr;
        using std::wcsstr;

        using std::wcstol;

        MENGINE_INLINE Char * strchrcat( Char * const _out, Char _ch )
        {
            size_t len = StdString::strlen( _out );

            _out[len] = _ch;
            _out[len + 1] = '\0';

            return _out;
        }

        MENGINE_INLINE Char * strchrcat_safe( Char * const _out, Char _ch, size_t _capacity )
        {
            size_t len = StdString::strlen( _out );

            if( len >= _capacity )
            {
                return _out;
            }

            _out[len] = _ch;
            _out[len + 1] = '\0';

            return _out;
        }

        MENGINE_INLINE WChar * wcschrcat( WChar * const _out, WChar _ch )
        {
            size_t len = StdString::wcslen( _out );
            _out[len] = _ch;
            _out[len + 1] = L'\0';

            return _out;
        }

        MENGINE_INLINE Char * strnchrcat( Char * const _out, Char _ch, size_t _count )
        {
            size_t len = StdString::strlen( _out );

            if( len + 1 >= _count )
            {
                return _out;
            }

            _out[len] = _ch;
            _out[len + 1] = '\0';

            return _out;
        }

        MENGINE_INLINE WChar * wcsnchrcat( WChar * const _out, WChar _ch, size_t _count )
        {
            size_t len = StdString::wcslen( _out );

            if( len + 1 >= _count )
            {
                return _out;
            }

            _out[len] = _ch;
            _out[len + 1] = L'\0';

            return _out;
        }

        MENGINE_INLINE Char * strcpy_safe( Char * const _out, const Char * _in, size_t _capacity )
        {
            if( _capacity == 0 )
            {
                return _out;
            }

            StdString::strncpy( _out, _in, _capacity );

            _out[_capacity] = '\0';

            return _out;
        }

        MENGINE_INLINE Char * strzcpy_safe( Char * const _out, const Char * _in, size_t _size, size_t _capacity )
        {
            if( _capacity == 0 )
            {
                return _out;
            }

            if( _size >= _capacity )
            {
                _size = _capacity;
            }

            StdString::strncpy( _out, _in, _size );

            _out[_size] = '\0';

            return _out;
        }

        MENGINE_INLINE Char * strcat_safe( Char * const _out, const Char * _in, size_t _capacity )
        {
            size_t len = StdString::strlen( _out );

            if( len >= _capacity )
            {
                return _out;
            }

            StdString::strncpy( _out + len, _in, _capacity - len );

            _out[_capacity] = '\0';

            return _out;
        }

        MENGINE_INLINE Char * strzcat_safe( Char * const _out, const Char * _in, size_t _size, size_t _capacity )
        {
            size_t len = StdString::strlen( _out );

            if( len >= _capacity )
            {
                return _out;
            }

            size_t capacity_len = _capacity - len;

            if( _size >= capacity_len )
            {
                _size = capacity_len;
            }

            StdString::strncpy( _out + len, _in, _size );

            _out[_capacity] = '\0';

            return _out;
        }

        using std::memcpy;
        using std::memset;
        using std::memmove;
        using std::memcmp;
    }
}

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRCPY_STATIC
#define MENGINE_STRCPY_STATIC(a, b, n) (Mengine::StdString::strcpy_safe((a), (b), (n)), MENGINE_STATIC_STRING_LENGTH(b))
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WCSICMP
#   if defined(MENGINE_PLATFORM_WINDOWS)
#       define MENGINE_WCSICMP(a,b) ::_wcsicmp((a), (b))
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRICMP
#if defined(MENGINE_COMPILER_MSVC)
#   define MENGINE_STRICMP(a, b) ::_stricmp((a), (b))
#elif defined(MENGINE_PLATFORM_LINUX) && defined(MENGINE_COMPILER_GCC)
#   define MENGINE_STRICMP(a, b) ::strcasecmp((a), (b))
#elif defined(MENGINE_PLATFORM_ANDROID)
#   define MENGINE_STRICMP(a, b) ::strcasecmp((a), (b))
#elif defined(MENGINE_PLATFORM_MACOS)
#   define MENGINE_STRICMP(a, b) ::strcasecmp((a), (b))
#elif defined(MENGINE_PLATFORM_IOS)
#   define MENGINE_STRICMP(a, b) ::strcasecmp((a), (b))
#else
#   define MENGINE_STRICMP(a, b) ::stricmp((a), (b))
#endif
#endif
//////////////////////////////////////////////////////////////////////////
