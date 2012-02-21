#	include "BinParser.h"
#	include "Loadable.h"

#	include "LogEngine.h"

namespace Menge
{
	namespace
	{
		class ExceptionBinParserStop			
		{
		};

		class ExceptionBinParserError
		{
		};
	}
#	ifdef MENGE_CONST_STRING
	//////////////////////////////////////////////////////////////////////////
	void operator >> ( ArchiveRead & ar, ConstString & _value )
	{
		std::string str;
		ar.readString( str );

		_value = ConstString(str);        
	}
#	endif
	//////////////////////////////////////////////////////////////////////////
	void operator >> ( ArchiveRead & ar, Resolution & _value )
	{
		size_t width;
		size_t height;
		ar.readPOD(width);
		ar.readPOD(height);

		_value.setWidth(width);
		_value.setHeight(height);
	}
	//////////////////////////////////////////////////////////////////////////
	void operator >> ( ArchiveRead & ar, ColourValue & _value )
	{
		float rgba[4];
		ar.readPOD( rgba[0] );
		ar.readPOD( rgba[1] );
		ar.readPOD( rgba[2] );
		ar.readPOD( rgba[3] );

		float coef = 1.f / 255.f;

		rgba[0] *= coef;
		rgba[1] *= coef;
		rgba[2] *= coef;
		rgba[3] *= coef;
		
		_value.setR( rgba[0] );
		_value.setG( rgba[1] );
		_value.setB( rgba[2] );
		_value.setA( rgba[3] );

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
	void operator >> ( ArchiveRead & ar, TVectorIndices & _value )
	{
		TVectorIndices::size_type size;
		ar >> size;

		_value.resize(size);

		for( TVectorIndices::iterator
			it = _value.begin(),
			it_end = _value.end();
		it != it_end;
		++it )
		{
			ar >> *it;
		}
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
	void BinParser::clear_()
	{
		m_vectorListeners[0] = 0;

		for( TVectorListeners::iterator
			it = m_vectorListeners.begin(),
			it_end = m_vectorListeners.end();
		it != it_end;
		++it )
		{
			delete *it;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool BinParser::run( BinParserListener * _listener )
	{
		m_reader.begin();

		if( m_reader.eof() == true )
		{
			return true;
		}

		char magic_number;
		m_reader.read( magic_number );

		if( magic_number != 42 )
		{
			return false;
		}

		//int version;
		//m_reader.read( version );

		//if( version != Menge::Protocol::version )
		//{
		//	return false;
		//}

		m_vectorListeners.push_back( _listener );

		try
		{
			this->readNode_();
		}
		catch( const ExceptionBinParserError & )
		{
			this->clear_();

			return false;
		}
		catch( const ExceptionBinParserStop & )
		{
			this->clear_();

			return true;
		}

		BinParserListener * listener = m_vectorListeners.back();
		this->notifyEndElement_();

		m_vectorListeners.pop_back();

		if( m_vectorListeners.empty() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void BinParser::stop()
	{
		throw ExceptionBinParserStop();
	}
	//////////////////////////////////////////////////////////////////////////
	void BinParser::addListener( BinParserListener * _listener )
	{
		m_vectorListeners.push_back( _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	size_t BinParser::readAttributeId()
	{            
		if( m_attributeCount == 0 )
		{
			return 0;
		}

		m_reader.read( m_elementId );

		--m_attributeCount;

		return m_elementId;
	}
	//////////////////////////////////////////////////////////////////////////
	void BinParser::readNode_()
	{      
		m_reader.read( m_elementId );
		m_reader.read( m_attributeCount );
        

		TVectorListeners::size_type listenersCount = m_vectorListeners.size();

		size_t debugAttributeCheck = m_attributeCount;

		notifyElement_();

		if( debugAttributeCheck != m_attributeCount && m_attributeCount != 0 )
		{
			MENGE_LOG_ERROR( "BinParser error: debugAttributeCheck != m_attributeCount && m_attributeCount != 0"
				);

			throw ExceptionBinParserError();
		}

		for( int i = 0; i != m_attributeCount; ++i )
		{
			readAttribute_();
		}

		size_t subNode;
		m_reader.read( subNode );
        

		for( int i = 0; i != subNode; ++i )
		{           
			this->readNode_();
		}

		if( listenersCount < m_vectorListeners.size() )
		{
			BinParserListener * listener = m_vectorListeners.back();
			this->notifyEndElement_();

			delete listener;
			m_vectorListeners.pop_back();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void BinParser::setDebugReadValue()
	{
		m_debugNeedReadValue = false;
	};
	//////////////////////////////////////////////////////////////////////////
	void BinParser::readAttribute_()
	{
		m_reader.read( m_elementId );
        
		m_debugNeedReadValue = true;

		notifyElement_();

		if( m_debugNeedReadValue == true )
		{
			MENGE_LOG_ERROR( "BinParser error: Need read value for element %d"
				, m_elementId
				);

			throw ExceptionBinParserError();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void BinParser::notifyElement_()
	{       
		BinParserListener * listener = m_vectorListeners.back();
		listener->onElement( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void BinParser::notifyEndElement_()
	{
		BinParserListener * listener = m_vectorListeners.back();
		listener->onEndElement();
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
	void BinParserListenerLoadable::onEndElement()
	{
		m_self->loaded();
	}
	//////////////////////////////////////////////////////////////////////////
	BinParserListener * binParserListenerLoadable( Loadable * _self )
	{
		return new BinParserListenerLoadable(_self);
	}
}