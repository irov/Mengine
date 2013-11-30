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
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize( const InputStreamInterfacePtr & _stream, bool & _version ) override;

    protected:
        virtual bool _initialize( bool & _version );

    public:
        bool setOptions( CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    public:
        const InputStreamInterfacePtr & getStream() const override;
		const SoundCodecDataInfo * getCodecDataInfo() const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        InputStreamInterfacePtr m_stream;

		SoundCodecOptions m_options;
		SoundCodecDataInfo m_dataInfo;
	};
}
