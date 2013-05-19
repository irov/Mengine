#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class StringizeServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("StringizeService")

	public:
		virtual ConstString stringize( const char * _str, size_t _size ) = 0;
	};


#   define STRINGIZE_SERVICE( serviceProvider )\
    (Helper::getService<Menge::StringizeServiceInterface>(serviceProvider))

    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeStringSize( ServiceProviderInterface * _serviceProvider, const char * _value, size_t _size )
        {
            ConstString cstr = STRINGIZE_SERVICE(_serviceProvider)
                ->stringize( _value, _size );

            return cstr;
        }
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeString( ServiceProviderInterface * _serviceProvider, const String & _value )
        {
            ConstString cstr = stringizeStringSize( _serviceProvider, _value.c_str(), _value.size() );

            return cstr;
        }
    }
}


