#	pragma once

#	include "Interface/CodecInterface.h"

namespace Menge
{
	class Decoder
		: virtual public DecoderInterface
	{
	public:
		Decoder( FileInputInterface * _stream );

	public:
		FileInputInterface * getStream() override;
		void release() override;

	protected:
		FileInputInterface * m_stream;
	};
}
