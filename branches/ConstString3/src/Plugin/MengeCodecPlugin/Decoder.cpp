#	include "Decoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Decoder::Decoder( FileInputInterface * _stream )
		: m_stream(_stream)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputInterface * Decoder::getStream()
	{
		return m_stream;
	}
	//////////////////////////////////////////////////////////////////////////
	void Decoder::release()
	{
		delete this;
	}
}
