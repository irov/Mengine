#   include "UnicodeService.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeService, Menge::UnicodeServiceInterface, Menge::UnicodeService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    UnicodeService::UnicodeService()
        : m_serviceProvider(NULL)
        , m_unicodeSystem(NULL)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UnicodeService::~UnicodeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void UnicodeService::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;

        m_unicodeSystem = UNICODE_SYSTEM(m_serviceProvider);
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * UnicodeService::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnicodeService::unicodeToUtf8Size( const wchar_t * _unicode, size_t _unicodeSize, size_t * _utf8Size )
    {
        if( m_unicodeSystem == NULL )
        {
            return false;
        }

        if( m_unicodeSystem->unicodeToUtf8Size( _unicode, _unicodeSize, _utf8Size ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnicodeService::unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size )
    {
        if( m_unicodeSystem == NULL )
        {
            return false;
        }
        
        if( m_unicodeSystem->unicodeToUtf8( _unicode, _unicodeSize, _utf8, _utf8Capacity, _utf8Size ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnicodeService::utf8ToUnicodeSize( const char * _utf8, size_t _utf8Size, size_t * _unicodeSize )
    {
        if( m_unicodeSystem == NULL )
        {
            return false;
        }

        if( m_unicodeSystem->utf8ToUnicodeSize( _utf8, _utf8Size, _unicodeSize ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnicodeService::utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize )
    {
        if( m_unicodeSystem == NULL )
        {
            return false;
        }
        
        if( m_unicodeSystem->utf8ToUnicode( _utf8, _utf8Size, _unicode, _unicodeCapacity, _unicodeSize ) == false )
        {
            return false;
        }

        return true;
    }
}