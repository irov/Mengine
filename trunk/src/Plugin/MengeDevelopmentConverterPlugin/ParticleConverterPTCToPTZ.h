#	pragma once

#	include "DevelopmentConverter.h"

namespace Menge
{
	class ParticleConverterPTCToPTZ
		: public DevelopmentConverter
	{
	public:
		ParticleConverterPTCToPTZ( ServiceProviderInterface * _serviceProvider );
		~ParticleConverterPTCToPTZ();

	public:
		bool initialize() override;

    public:
		bool convert() override;
        	
	private:
		bool convert_( const FilePath & _input, const FilePath & _output );
    };
}	// namespace Menge
