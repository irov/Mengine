#	include "StringizeService.h"

#	include "Core/Hash.h"

#	include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( StringizeService, Menge::StringizeServiceInterface, Menge::StringizeService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{	
    //////////////////////////////////////////////////////////////////////////
    StringizeService::StringizeService()
        : m_serviceProvider(nullptr)
		, m_memory(0)
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
	bool StringizeService::stringize( const char * _str, size_t _size, bool _external, ConstString & _cstr )
	{
        stdex::const_string_holder * stringHolder = nullptr;
        
		if( _size == (size_t)-1 )
		{
			_size = strlen(_str);
		}

		if( _external == true )
		{
			ConstStringHolderStringExternal * string = m_poolStringExternal.createObjectT();
			string->setValue( _str, _size );

			stringHolder = string;
		}
		else if( _size < 16 )
		{
			ConstStringHolderStringSize * string = m_poolStringSize.createObjectT();            
			StringSizeBuffer16 * buffer = m_poolString16.createObjectT();
			const char * store_value = buffer->initialize( _str, _size );
			string->setValue( buffer, store_value, _size );

			stringHolder = string;

			m_memory += 16;
		}
		else if( _size < 32 )
		{
			ConstStringHolderStringSize * string = m_poolStringSize.createObjectT();            
			StringSizeBuffer32 * buffer = m_poolString32.createObjectT();
			const char * store_value = buffer->initialize( _str, _size );
			string->setValue( buffer, store_value, _size );

			stringHolder = string;

			m_memory += 32;
		}
		else if( _size < 64 )
		{
			ConstStringHolderStringSize * string = m_poolStringSize.createObjectT();            
			StringSizeBuffer64 * buffer = m_poolString64.createObjectT();
			const char * store_value = buffer->initialize( _str, _size );
			string->setValue( buffer, store_value, _size );

			stringHolder = string;

			m_memory += 64;
		}
		else if( _size < 128 )
		{
			ConstStringHolderStringSize * string = m_poolStringSize.createObjectT();            
			StringSizeBuffer128 * buffer = m_poolString128.createObjectT();
			const char * store_value = buffer->initialize( _str, _size );
			string->setValue( buffer, store_value, _size );

			stringHolder = string;

			m_memory += 128;
		}
		else if( _size < 256 )
		{
			ConstStringHolderStringSize * string = m_poolStringSize.createObjectT();            
			StringSizeBuffer256 * buffer = m_poolString256.createObjectT();
			const char * store_value = buffer->initialize( _str, _size );
			string->setValue( buffer, store_value, _size );

			stringHolder = string;

			m_memory += 256;
		}
        else
        {
            ConstStringHolderStringSTL * stringSTL = m_poolStringSTL.createObjectT();
            stringSTL->setValue( _str, _size );

            stringHolder = stringSTL;

			m_memory += _size;
        }

        _cstr = ConstString(stringHolder);

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void StringizeService::stringizeLocal( const char * _str, size_t _size, ConstString & _cstr, ConstStringHolderLocal & _holder )
	{
		ConstString::hash_type hash = Helper::makeHash( _str, _size );

		_holder.setup( _str, _size, hash );

		_cstr = ConstString(&_holder);
	}
}
