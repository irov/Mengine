#	include "Win32UnicodeSystem.h"

#   include "Interface/WindowsLayerInterface.h"

#	include "Logger/Logger.h"

#   include "WindowsLayer/WindowsIncluder.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeSystem, Menge::UnicodeSystemInterface, Menge::Win32UnicodeSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32UnicodeSystem::Win32UnicodeSystem()
		: m_serviceProvider(nullptr)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void Win32UnicodeSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Win32UnicodeSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32UnicodeSystem::unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size )
    {
#   if (WINVER >= 0x0600)
        DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
#   else
        DWORD dwConversionFlags = 0;
#   endif

        int int_unicodeSize = static_cast<int>(_unicodeSize);
        int int_utf8Capacity = static_cast<int>(_utf8Capacity);

        if( int_unicodeSize == UNICODE_UNSIZE && int_utf8Capacity != 0 )
        {
            ++int_utf8Capacity;
        }

        int utf8_size = ::WideCharToMultiByte(
            CP_UTF8
            , dwConversionFlags
            , _unicode
            , int_unicodeSize
            , _utf8
            , int_utf8Capacity
            , NULL
            , NULL
            );

        if( utf8_size == 0 && _unicodeSize != 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32UnicodeSystem::unicodeToUtf8 %ls WideCharToMultiByte 0"
                , _unicode
                );

            return false;
        }

        if( int_unicodeSize == UNICODE_UNSIZE )
        {
            --utf8_size;
        }

        if( _utf8Size != nullptr )
        {
            size_t u_utf8_size = static_cast<size_t>(utf8_size);
            *_utf8Size = u_utf8_size;
        }

        if( _utf8 != nullptr && _utf8Capacity != 0 )
        {
            _utf8[ utf8_size ] = '\0';
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32UnicodeSystem::utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode )
	{
        int int_utf8Size = static_cast<int>(_utf8Size);
        int int_unicodeCapacity = static_cast<int>(_unicodeCapacity);

        if( int_utf8Size == UNICODE_UNSIZE && int_unicodeCapacity != 0 )
        {
            ++int_unicodeCapacity;
        }

        int wc_size = ::MultiByteToWideChar(
            CP_UTF8
            , MB_ERR_INVALID_CHARS
            , _utf8
            , int_utf8Size
            , _unicode
            , int_unicodeCapacity
            );
        
        if( wc_size == 0 && _utf8Size != 0 )
        {
            DWORD err = GetLastError();

            WString wstr_err;
            WINDOWSLAYER_SERVICE(m_serviceProvider)
                ->makeFormatMessage(err, wstr_err);

            LOGGER_ERROR(m_serviceProvider)("Win32UnicodeSystem::utf8ToUnicode %s MultiByteToWideChar 0 %ls"
                , _utf8
                , wstr_err.c_str()
                );

            return false;
        }

        if( int_utf8Size == UNICODE_UNSIZE )
        {
            --wc_size;
        }

        if( _sizeUnicode != nullptr )
        {
            size_t u_wc_size = static_cast<size_t>(wc_size);
            *_sizeUnicode = u_wc_size;
        }

        if( _unicode != nullptr && _unicodeCapacity != 0 )
        {
            _unicode[ wc_size ] = L'\0';
        }

		return true;
	}
}