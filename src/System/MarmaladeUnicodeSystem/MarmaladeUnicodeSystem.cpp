#	include "MarmaladeUnicodeSystem.h"

#	include "Logger/Logger.h"

#   include <IwUTF8.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeSystem, Menge::MarmaladeUnicodeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    const int iwutf8_invalid_cast = -1;
	//////////////////////////////////////////////////////////////////////////
	MarmaladeUnicodeSystem::MarmaladeUnicodeSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeUnicodeSystem::unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * _utf8, size_t _utf8Capacity, size_t * _utf8Size )
    {
		if( _unicodeSize == UNICODE_UNSIZE && _utf8Capacity != 0 )
		{
			_utf8Capacity += 1;
		}

        int utf8_size = IwWideCharToUTF8( _unicode, _unicodeSize, _utf8, _utf8Capacity );

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
            size_t u_utf8_size = (size_t)utf8_size;
            *_utf8Size = u_utf8_size;
        }
        
		if( _utf8 != nullptr && _utf8Capacity != 0 )
        {
            _utf8[utf8_size] = '\0';
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeUnicodeSystem::utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode )
	{		
		if( _utf8Size == UNICODE_UNSIZE && _unicodeCapacity != 0 )
		{
			_unicodeCapacity += 1;
		}

        int wc_size = IwUTF8ToWideChar( _utf8, _utf8Size, _unicode, _unicodeCapacity);

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
            size_t u_wc_size = (size_t)wc_size;
            *_sizeUnicode = u_wc_size;
        }

        if( _unicode != nullptr && _unicodeCapacity != 0 )
        {
            _unicode[wc_size] = L'\0';
        }

		return true;
	}
}