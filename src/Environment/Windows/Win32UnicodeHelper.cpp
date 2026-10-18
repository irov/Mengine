#include "Win32UnicodeHelper.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool Win32Utf8ToUnicode( const String & _utf8, WString * const _unicode )
        {
            const Char * buffer = _utf8.c_str();
            size_t size = _utf8.size();
            if( Helper::Win32Utf8ToUnicode( buffer, size, _unicode ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32Utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WString * const _unicode )
        {
            _unicode->clear();

            if( _utf8Size == 0 )
            {
                return true;
            }

            int size = static_cast<int>(_utf8Size);
            int length = ::MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, _utf8, size, nullptr, 0 );

            if( length == 0 )
            {
                return false;
            }

            _unicode->resize( length );
            WChar * output = _unicode->data();
            int converted = ::MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, _utf8, size, output, length );

            if( converted != length )
            {
                _unicode->clear();

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32UnicodeToUtf8( const WChar * _unicode, String * const _utf8 )
        {
            _utf8->clear();
            int length = ::WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, _unicode, -1, nullptr, 0, nullptr, nullptr );

            if( length == 0 )
            {
                return false;
            }

            _utf8->resize( length );
            Char * output = _utf8->data();
            int converted = ::WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, _unicode, -1, output, length, nullptr, nullptr );

            if( converted != length )
            {
                _utf8->clear();

                return false;
            }

            _utf8->resize( length - 1 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32UnicodeToUtf8( const WChar * _unicode, Char * const _utf8, size_t _capacity )
        {
            if( _capacity == 0 )
            {
                return false;
            }

            _utf8[0] = '\0';
            int capacity = static_cast<int>(_capacity);
            int converted = ::WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, _unicode, -1, _utf8, capacity, nullptr, nullptr );

            if( converted == 0 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
