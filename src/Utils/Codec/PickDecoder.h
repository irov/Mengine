#	pragma once

#	include "Interface/PickCodecInterface.h"

#	include "Codec/Decoder.h"

namespace Menge
{
	class PickDecoder
		: public Decoder<PickDecoderInterface>
	{
	public:
		PickDecoder();

    public:
        bool setOptions( CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

	public:
		void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
		const PickCodecDataInfo * getCodecDataInfo() const override;

	protected:
		PickCodecOptions m_options;
		PickCodecDataInfo m_dataInfo;
	};
}
