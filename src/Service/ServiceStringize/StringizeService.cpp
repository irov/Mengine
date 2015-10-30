#	include "StringizeService.h"

#	include "Core/Hash.h"

#	include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( StringizeService, Menge::StringizeService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{	
    //////////////////////////////////////////////////////////////////////////
    StringizeService::StringizeService()
        : m_memory(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StringizeService::~StringizeService()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool StringizeService::stringize( const char * _str, size_t _size, bool _external, ConstString & _cstr )
	{
        stdex::const_string_holder * stringHolder = nullptr;
        
		if( _size == (size_t)-1 )
		{
			_size = strlen(_str);
		}

		long hash = Helper::makeHash( _str, _size );

		if( _external == true )
		{
			ConstStringHolderStringExternal * string = m_poolStringExternal.createObject();

			string->setValue( _str, _size, hash );

			stringHolder = string;
		}
		else
		{
			ConstStringHolderMemory * string = m_poolHolderStringMemory.createObject();

			string->setValue( _str, _size, hash );

			m_memory += _size + 1;

			stringHolder = string;
		}

        _cstr = ConstString(stringHolder);

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void StringizeService::stringizeLocal( const char * _str, size_t _size, ConstString & _cstr, ConstStringHolderLocal & _holder )
	{
		long hash = Helper::makeHash( _str, _size );

		_holder.setup( _str, _size, hash );

		_cstr = ConstString(&_holder);
	}
}
