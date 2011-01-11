#	include "Encoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Encoder::Encoder( CodecServiceInterface * _service, OutputStreamInterface * _stream )
		: m_service(_service)
		, m_stream(_stream)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterface * Encoder::getStream() const
	{
		return m_stream;
	}
	//////////////////////////////////////////////////////////////////////////
	void Encoder::release()
	{
		delete this;
	}
}
