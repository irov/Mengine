#	include "PosixUnicodeSystem.h"

#	include "Logger/Logger.h"

#	include <wchar.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeSystem, Menge::PosixUnicodeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    const size_t iwutf8_invalid_cast = (size_t)-1;
    //////////////////////////////////////////////////////////////////////////
    PosixUnicodeSystem::PosixUnicodeSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PosixUnicodeSystem::unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * _utf8, size_t _utf8Capacity, size_t * _utf8Size )
    {
        if( _unicodeSize == UNICODE_UNSIZE && _utf8Capacity != 0 )
        {
            _utf8Capacity += 1;
        }

        mbstate_t state = mbstate_t();
        size_t utf8_size = wcsrtombs( _utf8, &_unicode, _unicodeSize, &state );

        if( utf8_size == iwutf8_invalid_cast )
        {
            return false;
        }

        if( _utf8Capacity == UNICODE_UNSIZE )
        {
            utf8_size -= 1;
        }

        if( _utf8Size != nullptr )
        {
            *_utf8Size = utf8_size;
        }
        
        if( _utf8 != nullptr && _utf8Capacity != 0 )
        {
            _utf8[utf8_size] = '\0';
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PosixUnicodeSystem::utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode )
    {		
        if( _utf8Size == UNICODE_UNSIZE && _unicodeCapacity != 0 )
        {
            _unicodeCapacity += 1;
        }

        mbstate_t state = mbstate_t();
        size_t wc_size = mbsrtowcs( _unicode, &_utf8, _utf8Size, &state );

        if( wc_size == iwutf8_invalid_cast )
        {
            return false;
        }

        if( _utf8Size == UNICODE_UNSIZE )
        {
            wc_size -= 1;
        }

        if( _sizeUnicode != nullptr )
        {
            *_sizeUnicode = wc_size;
        }

        if( _unicode != nullptr && _unicodeCapacity != 0 )
        {
            _unicode[wc_size] = L'\0';
        }

        return true;
    }
}