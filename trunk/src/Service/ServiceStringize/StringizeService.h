#	pragma once

#	include "Interface/StringizeInterface.h"

#   include "Factory/FactoryPool.h"

#   include "ConstStringHolderStringSTL.h"
#   include "ConstStringHolderString256.h"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	class StringizeService
		: public StringizeServiceInterface
	{
    public:
        StringizeService();
        ~StringizeService();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		ConstString stringize( const char * _str, size_t _size ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        typedef FactoryPool<ConstStringHolderString256, 256> FactoryPoolString256;
        FactoryPoolString256 m_poolString256;

        typedef FactoryPool<ConstStringHolderStringSTL, 64> FactoryPoolSTLString;
        FactoryPoolSTLString m_poolSTLString;
	};
}
