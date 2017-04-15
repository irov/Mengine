#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Core/ConstString.h"
#	include "Core/ConstStringHolder.h"
#	include "Core/FilePath.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"

#   include "Factory/FactorablePtr.h"

namespace Menge
{
	class StringizeServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("StringizeService")
			
	public:
		virtual void stringize( const Char * _str, ConstString::size_type _size, ConstString::hash_type _hash, ConstString & _cstr ) = 0;
		
    public:
		virtual void stringizeInternal(const Char * _str, ConstString::size_type _size, ConstString & _cstr) = 0;
        virtual bool stringizeExternal( ConstStringHolder * _holder, ConstString & _cstr ) = 0;
	};

#   define STRINGIZE_SERVICE( serviceProvider )\
    ((Menge::StringizeServiceInterface*)SERVICE_GET(serviceProvider, Menge::StringizeServiceInterface))

    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeStringSizeHash( ServiceProviderInterface * _serviceProvider, const Char * _value, ConstString::size_type _size, ConstString::hash_type _hash )
        {
            ConstString cstr;
            STRINGIZE_SERVICE( _serviceProvider )
                ->stringize( _value, _size, _hash, cstr );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeStringSize( ServiceProviderInterface * _serviceProvider, const Char * _value, ConstString::size_type _size )
        {
			ConstString cstr = stringizeStringSizeHash( _serviceProvider, _value, _size, ((ConstString::hash_type)(-1)) );

			return cstr;
        }
		//////////////////////////////////////////////////////////////////////////
		inline ConstString stringizeString( ServiceProviderInterface * _serviceProvider, const Char * _value )
		{
			ConstString cstr = stringizeStringSize( _serviceProvider, _value, ((ConstString::size_type)(-1)) );

			return cstr;
		}
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeString( ServiceProviderInterface * _serviceProvider, const String & _value )
        {
            ConstString cstr = stringizeStringSize( _serviceProvider, _value.c_str(), _value.size() );

            return cstr;
        }
		//////////////////////////////////////////////////////////////////////////
		inline ConstString stringizeStringLocal(ServiceProviderInterface * _serviceProvider, const Char * _value, ConstString::size_type _size)
		{
			ConstString cstr;
			STRINGIZE_SERVICE(_serviceProvider)
				->stringizeInternal(_value, _size, cstr);

			return cstr;
		}
	}

#	define STRINGIZE_STRING_LOCAL( serviceProvider, str )\
	Helper::stringizeStringLocal( serviceProvider, str, (sizeof(str) - 1) )
}


