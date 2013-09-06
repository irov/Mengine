#	pragma once

#   include "Interface/ConverterInterface.h"

#	include "MovieFramePack.h"

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"


namespace Menge
{
	class MovieKeyConverter
        : public ConverterInterface
	{
	public:
		MovieKeyConverter( ServiceProviderInterface * _serviceProvider );
		~MovieKeyConverter();

	public:
		bool initialize() override;

	public:
		const String & getConvertExt() const override;

	public:
		bool convert() override;

	protected:
		bool loadFramePak_( MovieFramePack & _movieFramePack );
		bool writeFramePak_( MovieFramePack & _movieFramePack );

	public:
		void setOptions( ConverterOptions * _options ) override;

	public:
		void destroy() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConverterOptions m_options;

		String m_convertExt;		
	};
}