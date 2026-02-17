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

        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void strchrcat( Char * const _out, Char _ch )
        {
            assert( _out != nullptr && "strchrcat: output buffer is null" );

            size_t len = StdString::strlen( _out );

            _out[len] = _ch;
            _out[len + 1] = '\0';
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void strchrcat_safe( Char * const _out, Char _ch, size_t _capacity )
        {
            assert( _out != nullptr && "strchrcat_safe: output buffer is null" );

            if( _capacity == 0 )
            {
                return;
            }

            size_t len = StdString::strlen( _out );

            if( len >= _capacity - 1 )
            {
                return;
            }

            _out[len] = _ch;
            _out[len + 1] = '\0';
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void wcschrcat( WChar * const _out, WChar _ch )
        {
            assert( _out != nullptr && "wcschrcat: output buffer is null" );

            size_t len = StdString::wcslen( _out );

            _out[len] = _ch;
            _out[len + 1] = L'\0';
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void wcschrcat_safe( WChar * const _out, WChar _ch, size_t _capacity )
        {
            assert( _out != nullptr && "wcschrcat_safe: output buffer is null" );

            if( _capacity == 0 )
            {
                return;
            }

            size_t len = StdString::wcslen( _out );

            if( len >= _capacity - 1 )
            {
                return;
            }

            _out[len] = _ch;
            _out[len + 1] = L'\0';
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void strchrcpy_safe( Char * const _out, Char _ch, size_t _capacity )
        {
            assert( _out != nullptr && "strchrcpy_safe: output buffer is null" );

            if( _capacity < 2 )
            {
                return;
            }

            _out[0] = _ch;
            _out[1] = '\0';
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void strcpy_safe( Char * const _out, const Char * _in, size_t _capacity )
        {
            assert( _out != nullptr && "strcpy_safe: output buffer is null" );
            assert( _in != nullptr && "strcpy_safe: input buffer is null" );

            if( _capacity == 0 )
            {
                return;
            }

            Char * it = _out;

            for( ; *_in != '\0' && _capacity > 1; ++_in, ++it, --_capacity )
            {
                *it = *_in;
            }

            *it = '\0';
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void strzcpy_safe( Char * const _out, const Char * _in, size_t _size, size_t _capacity )
        {
            assert( _out != nullptr && "strzcpy_safe: output buffer is null" );
            assert( _in != nullptr && "strzcpy_safe: input buffer is null" );

            if( _size == MENGINE_UNKNOWN_SIZE )
            {
                StdString::strcpy_safe( _out, _in, _capacity );

                return;
            }

            if( _capacity == 0 )
            {
                return;
            }

            if( _size >= _capacity )
            {
                _size = _capacity - 1;
            }

            StdString::memcpy( _out, _in, _size );

            _out[_size] = '\0';
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void strcat_safe( Char * const _out, const Char * _in, size_t _capacity )
        {
            assert( _out != nullptr && "strcat_safe: output buffer is null" );
            assert( _in != nullptr && "strcat_safe: input buffer is null" );

            if( _capacity == 0 )
            {
                return;
            }

            size_t offset = StdString::strlen( _out );

            if( offset >= _capacity )
            {
                return;
            }

            size_t space = _capacity - offset;

            Char * it = _out + offset;

            for( ; *_in != '\0' && space > 1; ++_in, ++it, --space )
            {
                *it = *_in;
            }

            *it = '\0';
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void strzcat_safe( Char * const _out, const Char * _in, size_t _size, size_t _capacity )
        {
            assert( _out != nullptr && "strzcat_safe: output buffer is null" );
            assert( _in != nullptr && "strzcat_safe: input buffer is null" );

            if( _size == MENGINE_UNKNOWN_SIZE )
            {
                StdString::strcat_safe( _out, _in, _capacity );

                return;
            }

            if( _capacity == 0 )
            {
                return;
            }

            size_t offset = StdString::strlen( _out );

            if( offset >= _capacity )
            {
                return;
            }

            size_t space = _capacity - offset;

            if( _size >= space )
            {
                _size = space - 1;
            }

            StdString::memcpy( _out + offset, _in, _size );

            _out[offset + _size] = '\0';
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void wcscpy_safe( WChar * const _out, const WChar * _in, size_t _capacity )
        {
            assert( _out != nullptr && "wcscpy_safe: output buffer is null" );
            assert( _in != nullptr && "wcscpy_safe: input buffer is null" );

            if( _capacity == 0 )
            {
                return;
            }

            WChar * it = _out;

            for( ; *_in != L'\0' && _capacity > 1; ++_in, ++it, --_capacity )
            {
                *it = *_in;
            }

            *it = L'\0';
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void wcszcpy_safe( WChar * const _out, const WChar * _in, size_t _size, size_t _capacity )
        {
            assert( _out != nullptr && "wcszcpy_safe: output buffer is null" );
            assert( _in != nullptr && "wcszcpy_safe: input buffer is null" );

            if( _size == MENGINE_UNKNOWN_SIZE )
            {
                wcscpy_safe( _out, _in, _capacity );

                return;
            }

            if( _capacity == 0 )
            {
                return;
            }

            if( _size >= _capacity )
            {
                _size = _capacity - 1;
            }

            StdString::memcpy( _out, _in, _size * sizeof( WChar ) );

            _out[_size] = L'\0';
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE WChar * wcschrcpy_safe( WChar * const _out, WChar _ch, size_t _capacity )
        {
            assert( _out != nullptr && "wcschrcpy_safe: output buffer is null" );
            
            if( _capacity < 2 )
            {
                return _out;
            }

            _out[0] = _ch;
            _out[1] = L'\0';

            return _out;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void wcscat_safe( WChar * const _out, const WChar * _in, size_t _capacity )
        {
            assert( _out != nullptr && "wcscat_safe: output buffer is null" );
            assert( _in != nullptr && "wcscat_safe: input buffer is null" );

            if( _capacity == 0 )
            {
                return;
            }

            size_t offset = StdString::wcslen( _out );

            if( offset >= _capacity )
            {
                return;
            }

            size_t space = _capacity - offset;

            WChar * it = _out + offset;

            for( ; *_in != L'\0' && space > 1; ++_in, ++it, --space )
            {
                *it = *_in;
            }

            *it = L'\0';
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void wcszcat_safe( WChar * const _out, const WChar * _in, size_t _size, size_t _capacity )
        {
            assert( _out != nullptr && "wcszcat_safe: output buffer is null" );
            assert( _in != nullptr && "wcszcat_safe: input buffer is null" );

            if( _size == MENGINE_UNKNOWN_SIZE )
            {
                StdString::wcscat_safe( _out, _in, _capacity );

                return;
            }

            if( _capacity == 0 )
            {
                return;
            }

            size_t offset = StdString::wcslen( _out );

            if( offset >= _capacity )
            {
                return;
            }

            size_t space = _capacity - offset;

            if( _size >= space )
            {
                _size = space - 1;
            }

            StdString::memcpy( _out + offset, _in, _size * sizeof( WChar ) );

            _out[offset + _size] = L'\0';
        }
        //////////////////////////////////////////////////////////////////////////
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
