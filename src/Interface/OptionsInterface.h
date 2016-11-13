#	pragma once

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "Interface/ServiceInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class OptionsSystemInterface
		: public ServiceInterface
	{ 
		SERVICE_DECLARE( "OptionsSystem" )

	public:
		virtual bool getOptions( TVectorString & _options ) const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
#   define OPTIONS_SYSTEM( serviceProvider )\
    ((Menge::OptionsSystemInterface *)SERVICE_GET(serviceProvider, Menge::OptionsSystemInterface))
	//////////////////////////////////////////////////////////////////////////
	class OptionsServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("OptionsService")

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

