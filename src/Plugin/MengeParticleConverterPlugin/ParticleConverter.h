#	pragma once

#	include "Interface/ConverterInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/LogSystemInterface.h"

namespace Menge
{
	class ParticleConverter
        : public ConverterInterface
	{
	public:
		ParticleConverter( ServiceProviderInterface * _serviceProvider );
		virtual ~ParticleConverter();

	public:
		virtual void setOptions( ConverterOptions * _options ) override;

    public:
		virtual void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		ConverterOptions m_options;
	};
}
