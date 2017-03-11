#	pragma once

#	include "Interface/StringizeInterface.h"

#   include "Core/ServiceBase.h"

#   include "Factory/Factory.h"

#   include "ConstStringHolderMemory.h"

#   include "stdex/intrusive_list.h"

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
		bool _initialize() override;
		void _finalize() override;

	public:
		void stringize( const char * _str, size_t _size, ConstString::hash_type _hash, ConstString & _cstr ) override;

	public:
		void stringizeInternal(const Char * _str, ConstString::size_type _size, ConstString & _cstr) override;
        bool stringizeExternal( ConstStringHolder * _holder, ConstString & _cstr ) override;
		
    protected:	
		FactoryPtr m_factoryHolderStringMemory;

        typedef stdex::intrusive_list<ConstStringHolder> TIntrusiveListConstStringHolder;
        TIntrusiveListConstStringHolder m_holdres[4096];

		struct InternalHolder
		{
			const char * str;
			ConstStringHolderPtr holder;
		};

		InternalHolder m_internals[257][8];
        
    protected:		
        ConstStringHolder * testHolder_( const char * _str, ConstString::size_type _size, ConstString::hash_type _hash );
		ConstStringHolder * stringizeHolder_(const char * _str, size_t _size, ConstString::hash_type _hash);
		void addHolder_( ConstStringHolder * _holder, ConstString::hash_type _hash );
        TIntrusiveListConstStringHolder & getList_( ConstStringHolder::hash_type _hash );
	};
}
