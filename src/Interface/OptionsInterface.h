#	pragma once

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "Interface/ServiceInterface.h"

namespace Menge
{
	class OptionsInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("Options")

	public:
		virtual bool hasOption( const Char * _key ) const = 0;
	};

#   define OPTIONS_SERVICE( serviceProvider )\
    ((Menge::OptionsInterface *)SERVICE_GET(serviceProvider, Menge::OptionsInterface))

#	define HAS_OPTIONS( serviceProvider, key )\
	(OPTIONS_SERVICE( serviceProvider )->hasOption( key ) == true)

#	define APSENT_OPTIONS( serviceProvider, key )\
	(OPTIONS_SERVICE( serviceProvider )->hasOption( key ) == false)
}

