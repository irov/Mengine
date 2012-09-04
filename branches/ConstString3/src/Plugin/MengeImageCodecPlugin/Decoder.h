#	pragma once

#	include "Interface/CodecInterface.h"

namespace Menge
{
	class Decoder
		: virtual public DecoderInterface
	{
	public:
		Decoder( CodecServiceInterface * _service, InputStreamInterface * _stream );
		virtual ~Decoder();

	public:		
		void destroy() override;

	protected:
		CodecServiceInterface * m_service;
		InputStreamInterface * m_stream;
	};
}
