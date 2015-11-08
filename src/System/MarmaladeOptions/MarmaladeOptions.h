#	pragma once

#	include "Interface/OptionsInterface.h"

#	include "Config/Char.h"

namespace Menge
{
#	ifndef MENGINE_OPTIONS_KEY_SIZE
#	define MENGINE_OPTIONS_KEY_SIZE 32
#	endif

	class MarmaladeOptions
		: public ServiceBase<OptionsInterface>
	{
	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool hasOption( const Char * _key ) const override;

	protected:
		struct Option
		{
			Char key[MENGINE_OPTIONS_KEY_SIZE];
		};

		typedef stdex::vector<Option> TVectorOptions;
		TVectorOptions m_options;
	};
}


