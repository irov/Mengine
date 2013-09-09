#	pragma once

#	include "Interface/ConverterInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/LogSystemInterface.h"

namespace Menge
{
	class DevelopmentConverter
        : public ConverterInterface
	{
	public:
		DevelopmentConverter( ServiceProviderInterface * _serviceProvider );
		virtual ~DevelopmentConverter();

	public:
		const String & getConvertExt() const override;

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
