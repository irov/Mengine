#	pragma once

#	include "Interface/VideoCodecInterface.h"

#	include "Codec/Decoder.h"

namespace Menge
{
	class VideoDecoder
        : public Decoder<VideoDecoderInterface>
	{
	public:
		VideoDecoder();

    public:
        bool setOptions( const CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

	public:
		void setCodecDataInfo( const CodecDataInfo * ) override;
		const VideoCodecDataInfo * getCodecDataInfo() const override;
        
    protected:
		VideoCodecOptions m_options;
		VideoCodecDataInfo m_dataInfo;
	};
}
