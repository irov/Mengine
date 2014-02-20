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
        bool initialize( const InputStreamInterfacePtr & _stream ) override;

    protected:
        virtual bool _initialize();

    public:
        bool setOptions( CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    public:
        const InputStreamInterfacePtr & getStream() const override;

	public:
		void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
		const SoundCodecDataInfo * getCodecDataInfo() const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        InputStreamInterfacePtr m_stream;

		SoundCodecOptions m_options;
		SoundCodecDataInfo m_dataInfo;
	};
}
