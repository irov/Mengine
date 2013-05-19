#   include "UnicodeService.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UnicodeService, Menge::UnicodeServiceInterface, Menge::UnicodeService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    UnicodeService::UnicodeService()
        : m_serviceProvider(nullptr)
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
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * UnicodeService::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnicodeService::destroy()
    {
        delete this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnicodeService::unicodeToUtf8( const wchar_t * _unicode, size_t _unicodeSize, char * _utf8, size_t _utf8Capacity, size_t * _utf8Size )
    {  
        if( UNICODE_SYSTEM(m_serviceProvider)
            ->unicodeToUtf8( _unicode, _unicodeSize, _utf8, _utf8Capacity, _utf8Size ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnicodeService::utf8ToUnicode( const char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize )
    { 
        if( UNICODE_SYSTEM(m_serviceProvider)
            ->utf8ToUnicode( _utf8, _utf8Size, _unicode, _unicodeCapacity, _unicodeSize ) == false )
        {
            return false;
        }

        return true;
    }
}