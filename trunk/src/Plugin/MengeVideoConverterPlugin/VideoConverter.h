#	pragma once

#	include "Interface/ConverterInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/LogSystemInterface.h"

namespace Menge
{
	class VideoConverter
        : public ConverterInterface
	{
	public:
		VideoConverter( ServiceProviderInterface * _serviceProvider );
		virtual ~VideoConverter();

	public:
		virtual void setOptions( ConverterOptions * _options ) override;

    public:
		virtual void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		ConverterOptions m_options;
	};
}
