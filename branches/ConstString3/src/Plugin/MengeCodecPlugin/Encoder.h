#	pragma once

#	include "Interface/CodecInterface.h"

namespace Menge
{
	class Encoder
		: virtual public EncoderInterface
	{
	public:
		Encoder( FileOutputInterface * _stream );

	public:
		FileOutputInterface * getStream() override;
		void release() override;

	protected:
		FileOutputInterface * m_stream;
	};
}
