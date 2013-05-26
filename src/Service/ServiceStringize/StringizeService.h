#	pragma once

#	include "Interface/StringizeInterface.h"

#   include "Factory/FactoryPool.h"

#   include "ConstStringHolderStringSTL.h"
#   include "ConstStringHolderStringSize.h"

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
		bool stringize( const char * _str, size_t _size, ConstString & _cstr ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        typedef ConstStringHolderStringSize<256> ConstStringHolderString256;
        typedef FactoryPool<ConstStringHolderString256, 128> FactoryPoolString256;
        FactoryPoolString256 m_poolString256;

        typedef ConstStringHolderStringSize<128> ConstStringHolderString128;
        typedef FactoryPool<ConstStringHolderString128, 128> FactoryPoolString128;
        FactoryPoolString128 m_poolString128;

        typedef ConstStringHolderStringSize<64> ConstStringHolderString64;
        typedef FactoryPool<ConstStringHolderString64, 128> FactoryPoolString64;
        FactoryPoolString64 m_poolString64;

        typedef ConstStringHolderStringSize<32> ConstStringHolderString32;
        typedef FactoryPool<ConstStringHolderString32, 128> FactoryPoolString32;
        FactoryPoolString32 m_poolString32;

        typedef ConstStringHolderStringSize<16> ConstStringHolderString16;
        typedef FactoryPool<ConstStringHolderString16, 128> FactoryPoolString16;
        FactoryPoolString16 m_poolString16;

        typedef FactoryPool<ConstStringHolderStringSTL, 32> FactoryPoolSTLString;
        FactoryPoolSTLString m_poolSTLString;
	};
}
