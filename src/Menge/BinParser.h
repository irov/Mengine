#	pragma once

#	include "Utils/Core/Resolution.h"
#	include "Utils/Core/ColourValue.h"
#	include "Utils/Core/Viewport.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include "Math/vec4.h"
#	include "Math/mat3.h"

#	include "BinProtocol.h"

#	include "Utils/Archive/ArchiveRead.hpp"

namespace Menge
{
	class BinParser;

	class BinParserListener
	{
	public:
		virtual void onElement( BinParser * _parser ) = 0;
	};

	void operator >> ( ArchiveRead & ar, Resolution & _value );
	void operator >> ( ArchiveRead & ar, ColourValue & _value );
	void operator >> ( ArchiveRead & ar, Viewport & _value );

	void operator >> ( ArchiveRead & ar, mt::vec2f & _value );
	void operator >> ( ArchiveRead & ar, mt::vec3f & _value );
	void operator >> ( ArchiveRead & ar, mt::vec4f & _value );
	void operator >> ( ArchiveRead & ar, mt::mat3f & _value );

	class InputStreamInterface;

	class BinParser
	{
	public:
		BinParser();

	public:
		bool run( InputStreamInterface * _stream, BinParserListener * _listener );

	public:
		void addListener( BinParserListener * _listener );

	public:
		template<class T>
		void readAttribute( T & _value )
		{
			--m_attributeCount;
			m_reader >> _value;
		}

		template<class T>
		void readValue( T & _value )
		{
			m_reader >> _value;
		}

		template<class T, class C, class M>
		void readValueMethod( C * _self, M _method )
		{
			T value;
			m_reader >> value;

			(_self->*_method)( value );
		}

		template<class C, class M>
		void readValueMethodIf( C * _self, M _method1, M _method2 )
		{
			bool value;
			m_reader >> value;

			value ? (_self->*_method1)() : (_self->*_method2)();
		}

	public:
		inline bool checkNode( size_t _node ) const;
		inline bool checkAttribute( size_t _attribute ) const;

		inline int getAttributeCount() const;

	protected:
		void readNode_();
		void readAttribute_();
		void notifyListener_();

	protected:
		ArchiveRead m_reader;

		typedef  std::vector<BinParserListener *> TVectorListeners;
		TVectorListeners m_vectorListeners;

		int m_attributeCount;
		size_t m_nodeId;
		size_t m_attributeId;
	};

	//////////////////////////////////////////////////////////////////////////
	inline bool BinParser::checkNode( size_t _nodeId ) const
	{
		return m_nodeId == _nodeId;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool BinParser::checkAttribute( size_t _attribute ) const
	{
		return m_attributeId == _attribute;
	}
	//////////////////////////////////////////////////////////////////////////
	inline int BinParser::getAttributeCount() const
	{
		return m_attributeCount;
	}


	template<class C, class M>
	class BinParserListenerMethod
		: public BinParserListener
	{
	public:
		BinParserListenerMethod( C * _self, M _method )
			: m_self(_self)
			, m_method(_method)
		{
		}

	protected:
		void onElement( BinParser * _parser ) override
		{
			(m_self->*m_method)( _parser );
		}

	protected:
		C * m_self;
		M m_method;
	};

	template<class C, class M>
	BinParserListener * binParserListenerMethod( C * _self, M _method )
	{
		return new BinParserListenerMethod<C,M>(_self, _method);
	}

	template<class C, class M, class A1>
	class BinParserListenerMethod1
		: public BinParserListener
	{
	public:
		BinParserListenerMethod1( C * _self, M _method, A1 & _a1 )
			: m_self(_self)
			, m_method(_method)
			, m_a1(_a1)

		{
		}

	protected:
		void onElement( BinParser * _parser ) override
		{
			(m_self->*m_method)( _parser, m_a1 );
		}

	protected:
		C * m_self;
		M m_method;
		A1 & m_a1;
	};

	template<class C, class M, class A1>
	BinParserListener * binParserListenerMethod1( C * _self, M _method, A1 & _a1 )
	{
		return new BinParserListenerMethod1<C,M,A1>(_self, _method, _a1);
	}
}

#	define BIN_SWITCH_NODE( element )\
	for( bool xmlengine_parse_once = true ; xmlengine_parse_once == true; )\
	for( BinParser * xmlengine_element = element; xmlengine_parse_once == true; xmlengine_parse_once = false )\
	for( bool xmlengine_elif = false; xmlengine_elif == false; xmlengine_elif = true )\
	if( xmlengine_element != 0 )

#	define BIN_END_NODE()\
	else

#	define BIN_CASE_NODE( node )\
	if( xmlengine_elif == true ) continue; else\
	for( ; xmlengine_element->checkNode( node::id ) == true;\
	xmlengine_elif = true )

#	define BIN_FOR_EACH_ATTRIBUTES()\
	for( int xml_engine_attribute_it = 0,\
	xml_engine_attribute_it_end = xmlengine_element->getAttributeCount();\
	xml_engine_attribute_it != xml_engine_attribute_it_end;\
	++xml_engine_attribute_it )\
	for( bool xmlengine_elif = false; xmlengine_elif == false; xmlengine_elif = true )

#	define BIN_CASE_ATTRIBUTE( attribute, value )\
	if( xmlengine_elif == true ) continue; else\
	for( ; xmlengine_element->checkAttribute( attribute::id ) == true;\
	xmlengine_elif = true )\
	xmlengine_element->readAttribute<attribute::Type>( value )

#	define BIN_CASE_ATTRIBUTE_NODE( attribute, value )\
	if( xmlengine_elif == true ) continue; else\
	for( ; xmlengine_element->checkAttribute( attribute::id ) == true;\
	xmlengine_elif = true, xmlengine_element->readValue<attribute::Type>( value ) )

#	define BIN_CASE_ATTRIBUTE_NODE_METHOD( attribute, method )\
	if( xmlengine_elif == true ) continue; else\
	for( ; xmlengine_element->checkAttribute( attribute::id ) == true;\
	xmlengine_elif = true, xmlengine_element->readValueMethod<attribute::Type>( this, method ) )

#	define BIN_CASE_ATTRIBUTE_NODE_METHOD_IF( attribute, method1, method2 )\
	if( xmlengine_elif == true ) continue; else\
	for( ; xmlengine_element->checkAttribute( attribute::id ) == true;\
	xmlengine_elif = true, xmlengine_element->readValueMethodIf( this, method1, method2 ) )

#	define BIN_PARSE_ELEMENT( self, method )\
	do{ BinParserListener * listener = binParserListenerMethod( self, method ); xmlengine_element->addListener( listener ); } while(false)

#	define BIN_PARSE_ELEMENT_ARG1( self, method, arg1 )\
	do{ BinParserListener * listener = binParserListenerMethod1( self, method, arg1 ); xmlengine_element->addListener( listener ); } while(false)
