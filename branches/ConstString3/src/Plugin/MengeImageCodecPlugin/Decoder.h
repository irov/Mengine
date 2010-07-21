#	pragma once

#	include "Interface/CodecInterface.h"

namespace Menge
{
	class Decoder
		: virtual public DecoderInterface
	{
	public:
		Decoder( InputStreamInterface * _stream );
		virtual ~Decoder();

	public:
		InputStreamInterface * getStream() const override;
		void release() override;

	protected:
		InputStreamInterface * m_stream;
	};
}
