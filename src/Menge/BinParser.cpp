#	include "BinParser.h"

#	include "Interface/StreamInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void operator >> ( ArchiveRead & ar, Resolution & _value )
	{
		ar.readPOD( _value.m_width );
		ar.readPOD( _value.m_height );
	}
	//////////////////////////////////////////////////////////////////////////
	void operator >> ( ArchiveRead & ar, ColourValue & _value )
	{
		ar.readPOD( _value.r );
		ar.readPOD( _value.g );
		ar.readPOD( _value.b );
		ar.readPOD( _value.a );

		_value.invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void operator >> ( ArchiveRead & ar, Viewport & _value )
	{
		ar.readPOD( _value.begin );
		ar.readPOD( _value.end );
	}
	//////////////////////////////////////////////////////////////////////////
	void operator >> ( ArchiveRead & ar, mt::vec2f & _value )
	{
		ar.readPOD( _value.x );
		ar.readPOD( _value.y );
	}
	//////////////////////////////////////////////////////////////////////////
	void operator >> ( ArchiveRead & ar, mt::vec3f & _value )
	{
		ar.readPOD( _value.x );
		ar.readPOD( _value.y );
		ar.readPOD( _value.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void operator >> ( ArchiveRead & ar, mt::vec4f & _value )
	{
		ar.readPOD( _value.x );
		ar.readPOD( _value.y );
		ar.readPOD( _value.z );
		ar.readPOD( _value.w );
	}
	//////////////////////////////////////////////////////////////////////////
	void operator >> ( ArchiveRead & ar, mt::mat3f & _value )
	{
		ar >> _value.v0;
		ar >> _value.v1;
		ar >> _value.v2;
	}
	//////////////////////////////////////////////////////////////////////////
	BinParser::BinParser()
		: m_attributeCount(0)
		, m_nodeId(-1)
		, m_attributeId(-1)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool BinParser::run( InputStreamInterface * _stream, BinParserListener * _listener )
	{
		int size = _stream->size();

		Archive & buffer = m_reader.fill();
		buffer.resize(size);

		_stream->read( &buffer[0], size );

		m_reader.begin();

		m_vectorListeners.push_back( _listener );

		this->readNode_();

		if( m_vectorListeners.empty() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void BinParser::addListener( BinParserListener * _listener )
	{
		m_vectorListeners.push_back( _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void BinParser::readNode_()
	{
		m_reader.read( m_nodeId );

		m_attributeId = -1;

		m_reader.read( m_attributeCount );

		notifyListener_();

		for( int i = 0; i != m_attributeCount; ++i )
		{
			readAttribute_();
		}

		int subNode;
		m_reader.read( subNode );

		for( int i = 0; i != subNode; ++i )
		{
			readNode_();
		}

		BinParserListener * listener = m_vectorListeners.back();
		delete listener;
		m_vectorListeners.pop_back();
	}
	//////////////////////////////////////////////////////////////////////////
	void BinParser::readAttribute_()
	{
		m_reader.read( m_attributeId );

		notifyListener_();
	}
	//////////////////////////////////////////////////////////////////////////
	void BinParser::notifyListener_()
	{
		BinParserListener * listener = m_vectorListeners.back();
		listener->onElement( this );
	}
}