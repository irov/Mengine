#	pragma once

#	include "SoundConverter.h"

namespace Menge
{
	class ParticleConverterPTCToPTZ
		: public ParticleConverter
	{
	public:
		ParticleConverterPTCToPTZ( ServiceProviderInterface * _serviceProvider );
		~ParticleConverterPTCToPTZ();

	public:
		bool initialize() override;

    public:
        const String & getConvertExt() const override;

    public:
		bool convert() override;
        	
	private:
		bool convert_( const FilePath & _input, const FilePath & _output );

    protected:
        String m_convertExt;
    };
}	// namespace Menge
