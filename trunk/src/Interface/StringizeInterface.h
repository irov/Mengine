#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"

namespace Menge
{
	class StringizeServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("StringizeService")

	public:
		virtual bool stringize( const char * _str, size_t _size, bool _external, ConstString & _cstr ) = 0;
	};


#   define STRINGIZE_SERVICE( serviceProvider )\
    (Helper::getService<Menge::StringizeServiceInterface>(serviceProvider))

    namespace Helper
    {
		//////////////////////////////////////////////////////////////////////////
		inline ConstString stringizeStringExternal( ServiceProviderInterface * _serviceProvider, const char * _value, size_t _size )
		{
			ConstString cstr;
			STRINGIZE_SERVICE(_serviceProvider)
				->stringize( _value, _size, true, cstr );

			return cstr;
		}
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeStringSize( ServiceProviderInterface * _serviceProvider, const char * _value, size_t _size = (size_t)-1 )
        {
            ConstString cstr;
            STRINGIZE_SERVICE(_serviceProvider)
                ->stringize( _value, _size, false, cstr );

            return cstr;
        }
		//////////////////////////////////////////////////////////////////////////
		inline ConstString stringizeString( ServiceProviderInterface * _serviceProvider, const char * _value )
		{
			ConstString cstr = stringizeStringSize( _serviceProvider, _value, (size_t)-1 );

			return cstr;
		}
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeString( ServiceProviderInterface * _serviceProvider, const String & _value )
        {
            ConstString cstr = stringizeStringSize( _serviceProvider, _value.c_str(), _value.size() );

            return cstr;
        }
		//////////////////////////////////////////////////////////////////////////
		inline ConstString stringizeString( ServiceProviderInterface * _serviceProvider, const PathString & _value )
		{
			ConstString cstr = stringizeStringSize( _serviceProvider, _value.c_str(), _value.size() );

			return cstr;
		}
		//////////////////////////////////////////////////////////////////////////
		template<int code>
		const ConstString & stringizeStringLocal( const char * _str, size_t _size )
		{ 
			(void)code;

			static bool initialize = false;
			static ConstStringHolderLocal c_holder_local;
			static ConstString c_str;	
			if( initialize == false )
			{ 
				initialize = true;
				c_holder_local.setup( _str, _size );
				c_str = ConstString(&c_holder_local);
			}

			return c_str;
		}
	}

#	define CONST_STRING_LOCAL(str)\
	Helper::stringizeStringLocal< #@str >( #str, sizeof( #str ) )
}


