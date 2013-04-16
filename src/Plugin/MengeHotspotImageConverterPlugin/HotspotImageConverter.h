#	pragma once

#	include "Interface/ConverterInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/LogSystemInterface.h"

namespace Menge
{
	class HotspotImageConverter
        : public ConverterInterface
	{
	public:
		HotspotImageConverter( ServiceProviderInterface * _serviceProvider );
		virtual ~HotspotImageConverter();

	public:
		void setOptions( ConverterOptions * _options ) override;

    public:
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		ConverterOptions m_options;
	};
}
