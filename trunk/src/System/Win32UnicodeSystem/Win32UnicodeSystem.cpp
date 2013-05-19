#	include "Win32UnicodeSystem.h"

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
    void Win32UnicodeSystem::destroy()
    {
        delete this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32UnicodeSystem::unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size )
    {
#   if (WINVER >= 0x0600)
        DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
#   else
        DWORD dwConversionFlags = 0;
#   endif

        int utf8_size = ::WideCharToMultiByte(
            CP_UTF8
            , dwConversionFlags
            , _unicode
            , static_cast<int>(_unicodeSize)
            , _utf8
            , static_cast<int>(_utf8Capacity)
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

        if( _utf8Size != nullptr )
        {
            size_t u_utf8_size = static_cast<size_t>(utf8_size);
            *_utf8Size = u_utf8_size;
        }

        if( _utf8 != nullptr )
        {
            _utf8[ utf8_size ] = '\0';
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32UnicodeSystem::utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _sizeUnicode )
	{
        int wc_size = ::MultiByteToWideChar(
            CP_UTF8
            , MB_ERR_INVALID_CHARS
            , _utf8
            , static_cast<int>(_utf8Size)
            , _unicode
            , _unicodeCapacity
            );

        if( wc_size == 0 && _utf8Size != 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32UnicodeSystem::utf8ToUnicode %s MultiByteToWideChar 0"
                , _utf8
                );

            return false;
        }

        if( _sizeUnicode != nullptr )
        {
            size_t u_wc_size = static_cast<size_t>(wc_size);
            *_sizeUnicode = u_wc_size;
        }

        if( _unicode != nullptr )
        {
            _unicode[ wc_size ] = L'\0';
        }

		return true;
	}
}