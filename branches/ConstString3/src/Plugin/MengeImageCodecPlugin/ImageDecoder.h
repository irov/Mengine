#	pragma once

#	include "Decoder.h"

#	include "Interface/ImageCodecInterface.h"

namespace Menge
{
	class ImageDecoder
		: public Decoder
		, public ImageDecoderInterface
	{
	public:
		ImageDecoder( CodecServiceInterface * _service, InputStreamInterface * _stream );

	public:
		const ImageCodecDataInfo * getCodecDataInfo() const override;
		void setOptions( CodecOptions * _info ) override;

	protected:
		virtual void _invalidate();

	protected:
		ImageCodecOptions m_options;
		ImageCodecDataInfo m_dataInfo;
	};
}
