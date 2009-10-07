#	include "Decoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Decoder::Decoder()
		: m_stream(0)
		, m_valid( false )
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	void Decoder::initialize( FileInput* _stream, const String & _type )
	{
		m_stream = _stream;
		m_type = _type;

		this->_initialize();
	}
	//////////////////////////////////////////////////////////////////////////
	void Decoder::_initialize()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	FileInput* Decoder::getStream()
	{
		return m_stream;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & Decoder::getType() const
	{
		return m_type;
	}
}
