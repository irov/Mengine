#	pragma once

#	include "Interface/StringizeInterface.h"

#   include "Factory/FactoryStore.h"

#   include "ConstStringHolderStringExternal.h"
#   include "ConstStringHolderStringSTL.h"
#   include "ConstStringHolderWStringSTL.h"
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
		bool stringize( const char * _str, size_t _size, bool _external, ConstString & _cstr ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        typedef StringSizeBuffer<256> StringSizeBuffer256;
        typedef FactoryPoolStore<StringSizeBuffer256, 128> FactoryPoolString256;
        FactoryPoolString256 m_poolString256;

        typedef StringSizeBuffer<128> StringSizeBuffer128;
        typedef FactoryPoolStore<StringSizeBuffer128, 128> FactoryPoolString128;
        FactoryPoolString128 m_poolString128;

        typedef StringSizeBuffer<64> StringSizeBuffer64;
        typedef FactoryPoolStore<StringSizeBuffer64, 128> FactoryPoolString64;
        FactoryPoolString64 m_poolString64;

        typedef StringSizeBuffer<32> StringSizeBuffer32;
        typedef FactoryPoolStore<StringSizeBuffer32, 128> FactoryPoolString32;
        FactoryPoolString32 m_poolString32;

        typedef StringSizeBuffer<16> StringSizeBuffer16;
        typedef FactoryPoolStore<StringSizeBuffer16, 128> FactoryPoolString16;
        FactoryPoolString16 m_poolString16;

		typedef FactoryPoolStore<ConstStringHolderStringSize, 512> FactoryConstStringHolderStringSize;
		FactoryConstStringHolderStringSize m_poolStringSize;

        typedef FactoryPoolStore<ConstStringHolderStringSTL, 32> FactoryPoolStringSTL;
        FactoryPoolStringSTL m_poolStringSTL;

		typedef FactoryPoolStore<ConstStringHolderStringExternal, 128> FactoryPoolStringExternal;
		FactoryPoolStringExternal m_poolStringExternal;

		size_t m_memory;
	};
}
