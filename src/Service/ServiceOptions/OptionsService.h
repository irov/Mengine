#pragma once

#include "Interface/OptionsInterface.h"

#include "Core/ServiceBase.h"

#include "Config/Char.h"

namespace Mengine
{
#ifndef MENGINE_OPTIONS_KEY_SIZE
#	define MENGINE_OPTIONS_KEY_SIZE 32
#endif

#ifndef MENGINE_OPTIONS_VALUE_SIZE
#	define MENGINE_OPTIONS_VALUE_SIZE 32
#endif

	class OptionsService
		: public ServiceBase<OptionsServiceInterface>
	{
	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		void setArgs( const TVectorString & _args ) override;

	public:
		bool hasOption( const Char * _key ) const override;
		const Char * getOptionValue( const Char * _key ) const override;

	protected:
		struct Option
		{
			Char key[MENGINE_OPTIONS_KEY_SIZE];
			Char value[MENGINE_OPTIONS_VALUE_SIZE];
		};

		typedef stdex::vector<Option> TVectorOptions;
		TVectorOptions m_options;
	};
}


