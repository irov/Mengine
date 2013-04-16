#	include "StringizeService.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( StringizeService, Menge::StringizeServiceInterface, Menge::StringizeService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{	
    //////////////////////////////////////////////////////////////////////////
    StringizeService::StringizeService()
        : m_serviceProvider(NULL)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StringizeService::~StringizeService()
    { 
    }
    //////////////////////////////////////////////////////////////////////////
    void StringizeService::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * StringizeService::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	ConstString StringizeService::stringize( const char * _str, size_t _size )
	{
        ConstStringHolder * stringHolder = NULL;

        if( _size < 256 )
        {
            ConstStringHolderString256 * string256 = m_poolString256.createObjectT();
            string256->setValue( _str, _size );

            stringHolder = string256;
        }
        else
        {
            ConstStringHolderStringSTL * stringSTL = m_poolSTLString.createObjectT();
            stringSTL->setValue( _str, _size );

            stringHolder = stringSTL;
        }

        ConstString cs_str(stringHolder);

		return cs_str;
	}	
}
