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
		void setOptions( unsigned int _options ) override;

	protected:
		unsigned int m_options;
	};
}
