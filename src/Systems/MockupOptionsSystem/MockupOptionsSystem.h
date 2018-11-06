#	pragma once

#	include "Interface/OptionsInterface.h"

#	include "Config/Char.h"

namespace Mengine
{
	class MockupOptionsSystem
		: public ServiceBase<OptionsSystemInterface>
	{
	public:
		bool getOptions( Char * _options, size_t _capacity ) const override;
	};
}


