#	pragma once

#	include "Interface/SoundCodecInterface.h"

namespace Menge
{
	class SoundDecoder
		: public SoundDecoderInterface
	{
	public:
		SoundDecoder();

    public:
        ServiceProviderInterface * getServiceProvider() const;

    public:
        bool initialize( ServiceProviderInterface * _serviceProvider, InputStreamInterface * _stream ) override;

    protected:
        virtual bool _initialize();

    public:
        bool setOptions( CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    public:
        InputStreamInterface * getStream() const override;
		const SoundCodecDataInfo * getCodecDataInfo() const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        InputStreamInterface * m_stream;

		SoundCodecOptions m_options;
		SoundCodecDataInfo m_dataInfo;
	};
}
