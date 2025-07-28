#pragma once

#include "Config/Char.h"

#include "Config/StdAssert.h"

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

        using std::memcpy;
        using std::memset;
        using std::memmove;
        using std::memcmp;

        MENGINE_INLINE Char * strchrcat( Char * const _out, Char _ch )
        {
            size_t len = StdString::strlen( _out );

            _out[len] = _ch;
            _out[len + 1] = '\0';

            return _out;
        }

        MENGINE_INLINE Char * strchrcat_safe( Char * const _out, Char _ch, size_t _capacity )
        {
            if( _capacity == 0 )
            {
                assert( !"strchrcat_safe: output buffer overflow" );

                return _out;
            }

            size_t len = StdString::strlen( _out );

            if( len >= _capacity - 1 )
            {
                assert( !"strchrcat_safe: output buffer overflow" );

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

        MENGINE_INLINE WChar * wcschrcat_safe( WChar * const _out, WChar _ch, size_t _capacity )
        {
            if( _capacity == 0 )
            {
                assert( !"wcschrcat_safe: output buffer overflow" );

                return _out;
            }

            size_t len = StdString::wcslen( _out );

            if( len >= _capacity - 1 )
            {
                assert( !"wcschrcat_safe: output buffer overflow" );

                return _out;
            }

            _out[len] = _ch;
            _out[len + 1] = L'\0';

            return _out;
        }

        MENGINE_INLINE Char * strzcpy_safe( Char * const _out, const Char * _in, size_t _size, size_t _capacity )
        {
            if( _capacity == 0 )
            {
                assert( !"strzcpy_safe: output buffer overflow" );

                return _out;
            }

            if( _size == MENGINE_UNKNOWN_SIZE )
            {
                _size = StdString::strlen( _in );
            }

            if( _size >= _capacity )
            {
                assert( !"strzcpy_safe: output buffer overflow" );

                _size = _capacity - 1;
            }

            StdString::memcpy( _out, _in, _size * sizeof( Char ) );

            _out[_size] = '\0';

            return _out;
        }

        MENGINE_INLINE Char * strcpy_safe( Char * const _out, const Char * _in, size_t _capacity )
        {
            Char * result = StdString::strzcpy_safe( _out, _in, MENGINE_UNKNOWN_SIZE, _capacity );

            return result;
        }

        MENGINE_INLINE Char * strzcat_safe( Char * const _out, const Char * _in, size_t _size, size_t _capacity )
        {
            if( _capacity == 0 )
            {
                assert( !"strzcat_safe: output buffer overflow" );

                return _out;
            }

            size_t offset = StdString::strlen( _out );

            if( offset >= _capacity )
            {
                assert( !"strzcat_safe: output buffer overflow" );

                return _out;
            }

            size_t space = _capacity - offset - 1;

            if( _size == MENGINE_UNKNOWN_SIZE )
            {
                _size = StdString::strlen( _in );
            }

            if( _size > space )
            {
                assert( !"strzcat_safe: output buffer overflow" );

                _size = space;
            }

            StdString::memcpy( _out + offset, _in, _size * sizeof( Char ) );

            _out[offset + _size] = '\0';

            return _out;
        }

        MENGINE_INLINE Char * strcat_safe( Char * const _out, const Char * _in, size_t _capacity )
        {
            Char * result = StdString::strzcat_safe( _out, _in, MENGINE_UNKNOWN_SIZE, _capacity );

            return result;
        }

        MENGINE_INLINE WChar * wcszcpy_safe( WChar * const _out, const WChar * _in, size_t _size, size_t _capacity )
        {
            if( _capacity == 0 )
            {
                assert( !"wcszcpy_safe: output buffer overflow" );

                return _out;
            }

            if( _size == MENGINE_UNKNOWN_SIZE )
            {
                _size = StdString::wcslen( _in );
            }

            if( _size >= _capacity )
            {
                assert( !"wcszcpy_safe: output buffer overflow" );

                _size = _capacity - 1;
            }

            StdString::memcpy( _out, _in, _size * sizeof( WChar ) );

            _out[_size] = L'\0';

            return _out;
        }

        MENGINE_INLINE WChar * wcscpy_safe( WChar * const _out, const WChar * _in, size_t _capacity )
        {
            WChar * result = StdString::wcszcpy_safe( _out, _in, MENGINE_UNKNOWN_SIZE, _capacity );

            return result;
        }

        MENGINE_INLINE WChar * wcszcat_safe( WChar * const _out, const WChar * _in, size_t _size, size_t _capacity )
        {
            if( _capacity == 0 )
            {
                assert( !"wcszcat_safe: output buffer overflow" );

                return _out;
            }

            size_t offset = StdString::wcslen( _out );

            if( offset >= _capacity )
            {
                assert( !"wcszcat_safe: output buffer overflow" );

                return _out;
            }

            size_t space = _capacity - offset - 1;

            if( _size == MENGINE_UNKNOWN_SIZE )
            {
                _size = StdString::wcslen( _in );
            }

            if( _size > space )
            {
                assert( !"wcszcat_safe: output buffer overflow" );

                _size = space;
            }

            StdString::memcpy( _out + offset, _in, _size * sizeof( WChar ) );

            _out[offset + _size] = L'\0';

            return _out;
        }

        MENGINE_INLINE WChar * wcscat_safe( WChar * const _out, const WChar * _in, size_t _capacity )
        {
            WChar * result = StdString::wcszcat_safe( _out, _in, MENGINE_UNKNOWN_SIZE, _capacity );

            return result;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STRCPY_STATIC
#define MENGINE_STRCPY_STATIC(a, b, n) (Mengine::StdString::strcpy_safe((a), (b), (n)), MENGINE_STATIC_STRING_LENGTH(b))
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WCSICMP
#   if defined(MENGINE_PLATFORM_WINDOWS)
#       define MENGINE_WCSICMP(a, b) ::_wcsicmp((a), (b))
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
