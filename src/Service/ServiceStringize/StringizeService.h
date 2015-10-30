#	pragma once

#	include "Interface/StringizeInterface.h"

#   include "Factory/FactoryStore.h"

#   include "ConstStringHolderStringExternal.h"
#   include "ConstStringHolderMemory.h"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	class StringizeService
		: public ServiceBase<StringizeServiceInterface>
	{
    public:
        StringizeService();
        ~StringizeService();

	public:
		bool stringize( const char * _str, size_t _size, bool _external, ConstString & _cstr ) override;
		void stringizeLocal( const char * _str, size_t _size, ConstString & _cstr, ConstStringHolderLocal & _holder ) override;

    protected:	
		typedef FactoryPoolStore<ConstStringHolderMemory, 512> FactoryConstStringHolderMemory;
		FactoryConstStringHolderMemory m_poolHolderStringMemory;

		typedef FactoryPoolStore<ConstStringHolderStringExternal, 128> FactoryPoolStringExternal;
		FactoryPoolStringExternal m_poolStringExternal;

		size_t m_memory;
	};
}
