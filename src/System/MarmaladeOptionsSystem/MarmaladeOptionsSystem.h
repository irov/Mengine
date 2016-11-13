#	pragma once

#	include "Interface/OptionsInterface.h"

#	include "Config/Char.h"

namespace Menge
{
#	ifndef MENGINE_OPTIONS_KEY_SIZE
#	define MENGINE_OPTIONS_KEY_SIZE 32
#	endif

	class MarmaladeOptionsSystem
		: public ServiceBase<OptionsSystemInterface>
	{
	public:
		bool getOptions( TVectorString & _options ) const override;
	};
}


