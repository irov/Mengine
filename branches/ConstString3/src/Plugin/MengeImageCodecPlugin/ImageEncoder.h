#	pragma once

#	include "Encoder.h"

#	include "Interface/ImageCodecInterface.h"

namespace Menge
{
	class ImageEncoder
		: public Encoder
		, public ImageEncoderInterface
	{
	public:
		ImageEncoder( CodecServiceInterface * _service, OutputStreamInterface * _stream );

	public:
		void setOptions( CodecOptions * _info ) override;

	protected:
		virtual void _invalidate();

	protected:
		ImageCodecOptions m_options;
	};
}
