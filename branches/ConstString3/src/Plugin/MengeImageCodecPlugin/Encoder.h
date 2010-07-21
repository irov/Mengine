#	pragma once

#	include "Interface/CodecInterface.h"

namespace Menge
{
	class Encoder
		: virtual public EncoderInterface
	{
	public:
		Encoder( OutputStreamInterface * _stream );

	public:
		OutputStreamInterface * getStream() const override;
		void release() override;

	protected:
		OutputStreamInterface * m_stream;
	};
}
