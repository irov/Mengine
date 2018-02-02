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
#   define OPTIONS_SERVICE()\
    ((Menge::OptionsServiceInterface *)SERVICE_GET(Menge::OptionsServiceInterface))
	//////////////////////////////////////////////////////////////////////////
#	define HAS_OPTION( key )\
	OPTIONS_SERVICE()->hasOption( key )
	//////////////////////////////////////////////////////////////////////////
#	define GET_OPTION_VALUE( key )\
	OPTIONS_SERVICE()->getOptionValue( key )
}

