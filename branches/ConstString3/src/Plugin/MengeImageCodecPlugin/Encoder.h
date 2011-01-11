#	pragma once

#	include "Interface/CodecInterface.h"

namespace Menge
{
	class Encoder
		: virtual public EncoderInterface
	{
	public:
		Encoder( CodecServiceInterface * _service, OutputStreamInterface * _stream );

	public:
		OutputStreamInterface * getStream() const override;
		void release() override;

	protected:
		CodecServiceInterface * m_service;
		OutputStreamInterface * m_stream;
	};
}
