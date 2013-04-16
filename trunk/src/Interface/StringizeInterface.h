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

    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        inline ConstString stringizeStringSize( ServiceProviderInterface * _serviceProvider, const char * _value, size_t _size )
        {
            StringizeServiceInterface * stringizeService = 
                _serviceProvider->getServiceT<Menge::StringizeServiceInterface>("StringizeService");

            ConstString cstr = stringizeService->stringize( _value, _size );

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

#   define STRINGIZE_SERVICE( serviceProvider )\
    (serviceProvider->getServiceT<Menge::StringizeServiceInterface>("StringizeService"))

