#	pragma once

#	include "Interface/OptionsInterface.h"

#	include "Config/Char.h"

namespace Menge
{
	class Win32OptionsSystem
		: public ServiceBase<OptionsSystemInterface>
	{
	public:
		bool getOptions( TVectorString & _options ) const override;
	};
}


