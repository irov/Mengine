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
		ImageDecoder();

	public:
		const ImageCodecDataInfo * getCodecDataInfo() const override;
		void setOptions( unsigned int _options ) override;

	protected:
		ImageCodecDataInfo m_dataInfo;
		unsigned int m_options;
	};
}
