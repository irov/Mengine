#	pragma once

#	include "Interface/ConverterInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/LogSystemInterface.h"

namespace Menge
{
	class SoundConverter
        : public ConverterInterface
	{
	public:
		SoundConverter( ServiceProviderInterface * _serviceProvider );
		virtual ~SoundConverter();

	public:
		virtual void setOptions( ConverterOptions * _options ) override;

    public:
		virtual void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		ConverterOptions m_options;
	};
}
