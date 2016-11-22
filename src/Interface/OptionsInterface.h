#	pragma once

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "Interface/ServiceInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class OptionsServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("OptionsService")

	public:
		virtual void setArgs( const TVectorString & _args ) = 0;

	public:
		virtual bool hasOption( const Char * _key ) const = 0;
		virtual const Char * getOptionValue( const Char * _key ) const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
#   define OPTIONS_SERVICE( serviceProvider )\
    ((Menge::OptionsServiceInterface *)SERVICE_GET(serviceProvider, Menge::OptionsServiceInterface))
	//////////////////////////////////////////////////////////////////////////
#	define HAS_OPTION( serviceProvider, key )\
	OPTIONS_SERVICE( serviceProvider )->hasOption( key )
	//////////////////////////////////////////////////////////////////////////
#	define GET_OPTION_VALUE( serviceProvider, key )\
	OPTIONS_SERVICE( serviceProvider )->getOptionValue( key )
}

