#	include "BinParser.h"
#	include "Loadable.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void operator >> ( ArchiveRead & ar, ConstString & _value )
	{
		std::string str;
		ar.readString( str );

		_value = str;
	}
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
		ar >> _value.v0.x;
		ar >> _value.v0.y;
		ar >> _value.v1.x;
		ar >> _value.v1.y;
		ar >> _value.v2.x;
		ar >> _value.v2.y;
		
		_value.v0.z = 0.f;
		_value.v1.z = 0.f;
		_value.v2.z = 1.f;
	}
	//////////////////////////////////////////////////////////////////////////
	BinParserException::BinParserException( const std::string & _reason )
		: m_reason(_reason)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const char * BinParserException::what() const throw()
	{
		return m_reason.c_str();
	}
	//////////////////////////////////////////////////////////////////////////
	BinParser::BinParser( Archive::const_iterator _begin, Archive::const_iterator _end )
		: m_reader(_begin, _end)
		, m_attributeCount(0)
		, m_elementId(0)
		, m_debugNeedReadValue(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool BinParser::run( BinParserListener * _listener )
	{
		m_reader.begin();

		if( m_reader.eof() == true )
		{
			return true;
		}

		m_vectorListeners.push_back( _listener );

		try
		{
			this->readNode_();
		}
		catch( const BinParserException & )
		{
			return false;
		}

		m_vectorListeners.pop_back();

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
		m_reader.read( m_elementId );
		m_reader.read( m_attributeCount );

		TVectorListeners::size_type listenersCount = m_vectorListeners.size();

		std::size_t debugAttributeCheck = m_attributeCount;

		notifyListener_();

		if( debugAttributeCheck != m_attributeCount && m_attributeCount != 0 )
		{
			throw BinParserException( "invalid protocol" );
		}

		for( int i = 0; i != m_attributeCount; ++i )
		{
			readAttribute_();
		}

		std::size_t subNode;
		m_reader.read( subNode );

		for( int i = 0; i != subNode; ++i )
		{
			readNode_();
		}

		if( listenersCount < m_vectorListeners.size() )
		{
			BinParserListener * listener = m_vectorListeners.back();
			delete listener;
			m_vectorListeners.pop_back();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void BinParser::readAttribute_()
	{
		m_reader.read( m_elementId );

		m_debugNeedReadValue = true;

		notifyListener_();

		if( m_debugNeedReadValue == true )
		{
			throw BinParserException( "invalid protocol" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void BinParser::notifyListener_()
	{
		BinParserListener * listener = m_vectorListeners.back();
		listener->onElement( this );
	}
	//////////////////////////////////////////////////////////////////////////
	BinParserListenerLoadable::BinParserListenerLoadable( Loadable * _self )
		: m_self(_self)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void BinParserListenerLoadable::onElement( BinParser * _parser )
	{
		m_self->loader( _parser );
	}
	//////////////////////////////////////////////////////////////////////////
	BinParserListener * binParserListenerLoadable( Loadable * _self )
	{
		return new BinParserListenerLoadable(_self);
	}
}