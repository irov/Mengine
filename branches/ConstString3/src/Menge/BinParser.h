#	pragma once

#	include "Utils/Core/ConstString.h"
#	include "Utils/Core/Resolution.h"
#	include "Utils/Core/ColourValue.h"
#	include "Utils/Core/Viewport.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include "Math/vec4.h"
#	include "Math/mat3.h"

#	include "Vertex.h"

#	include "BinProtocol.h"
#	include "Core/ConstString.h"


#	include "Utils/Archive/ArchiveRead.hpp"


namespace Menge
{
	class Loadable;
	class BinParser;

	class BinParserListener
	{
	public:
		virtual ~BinParserListener(){};

	public:
		virtual void onElement( BinParser * _parser ) = 0;
		virtual void onEndElement() = 0;
	};

	void operator >> ( ArchiveRead & ar, String & _value );
	void operator >> ( ArchiveRead & ar, WString & _value );

	void operator >> ( ArchiveRead & ar, ConstString & _value );
	void operator >> ( ArchiveRead & ar, Resolution & _value );
	void operator >> ( ArchiveRead & ar, ColourValue & _value );
	void operator >> ( ArchiveRead & ar, Viewport & _value );

	void operator >> ( ArchiveRead & ar, mt::vec2f & _value );
	void operator >> ( ArchiveRead & ar, mt::vec3f & _value );
	void operator >> ( ArchiveRead & ar, mt::vec4f & _value );
	void operator >> ( ArchiveRead & ar, mt::mat3f & _value );

	void operator >> ( ArchiveRead & ar, Int2 & _value );
	void operator >> ( ArchiveRead & ar, Int4 & _value );

	void operator >> ( ArchiveRead & ar, TVectorIndices & _value );

	class BinParser
	{
	public:
		BinParser( Archive::const_iterator _begin, Archive::const_iterator _end );

	public:
		bool run( BinParserListener * _listener );
		void stop();

	public:
		void addListener( BinParserListener * _listener );

	public:
		template<class T>
		void readValue( T & _value )
		{
			m_reader >> _value;

			this->setDebugReadValue();
		}

		template<class T, class C, class M>
		void readValueMethod( C * _self, M _method )
		{
			T value;
			this->readValue( value );

			(_self->*_method)( value );
		}

		template<class T, class C, class M>
		bool readValueMethodCheck( C * _self, M _method )
		{
			T value;
			this->readValue( value );

			bool result = (_self->*_method)( value );

			return result;
		}

		template<class T, class C, class M, class A1>
		void readValueMethodArg1( C * _self, M _method, A1 & _a1 )
		{
			T value;
			this->readValue( value );

			(_self->*_method)( value, _a1 );
		}		

		template<class T, class C, class M, class CA1>
		void readValueMethodCArg1( C * _self, M _method, const CA1 & _ca1 )
		{
			T value;
			this->readValue( value );

			(_self->*_method)( value, _ca1 );
		}		

		template<class T1, class T2, class C, class M>
		void readValueMethodT( C * _self, M _method )
		{
			T1 value;
			this->readValue( value );

			(_self->*_method)( static_cast<T2>(value) );
		}


		template<class C, class M1, class M2>
		void readValueMethodIf( C * _self, M1 _method1, M2 _method2 )
		{
			bool value;
			this->readValue( value );

			if( value == true )
			{
				(_self->*_method1)();
			}
			else
			{ 
				(_self->*_method2)();
			}
		}

		template<class T, class F, class A1>
		void readValueFuncArg1( F _func, A1 & _arg1 )
		{
			T value;
			this->readValue( value );

			(*_func)( value, _arg1 );
		}

		template<class T>
		void readAttribute( T & _value )
		{
			this->readValue( _value );
		}
        
	public:		
		size_t readAttributeId()
		{            
			if( m_attributeCount == 0 )
			{
				return 0;
			}

			m_reader.read( m_elementId );

			--m_attributeCount;

			return m_elementId;
		}

	public:
		inline size_t getElementId() const;

		inline size_t getAttributeCount() const;

	protected:
		void setDebugReadValue();

	protected:
		void readNode_();
		void readAttribute_();
		
		void notifyElement_();
		void notifyEndElement_();

	protected:
		void clear_();

	protected:
		ArchiveRead m_reader;

		typedef  std::vector<BinParserListener *> TVectorListeners;
		TVectorListeners m_vectorListeners;

		size_t m_attributeCount;
		size_t m_elementId;

		bool m_debugNeedReadValue;
	};
	//////////////////////////////////////////////////////////////////////////
	inline size_t BinParser::getElementId() const
	{
		return m_elementId;
	}
	//////////////////////////////////////////////////////////////////////////
	inline size_t BinParser::getAttributeCount() const
	{
		return m_attributeCount;
	}
	//////////////////////////////////////////////////////////////////////////
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

		void onEndElement() override
		{
			//Empty
		}

	protected:
		C * m_self;
		M m_method;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M>
	BinParserListener * binParserListenerMethod( C * _self, M _method )
	{
		return new BinParserListenerMethod<C,M>(_self, _method);
	}
	//////////////////////////////////////////////////////////////////////////
	class BinParserListenerLoadable
		: public BinParserListener
	{
	public:
		BinParserListenerLoadable( Loadable * _self );

	protected:
		void onElement( BinParser * _parser ) override;
		void onEndElement() override;

	protected:
		Loadable * m_self;
	};
	//////////////////////////////////////////////////////////////////////////
	BinParserListener * binParserListenerLoadable( Loadable * _self );
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class A1>
	class BinParserListenerMethodArg1
		: public BinParserListener
	{
	public:
		BinParserListenerMethodArg1( C * _self, M _method, A1 & _a1 )
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

		void onEndElement() override
		{
			//Empty
		}

	protected:
		C * m_self;
		M m_method;

		A1 & m_a1;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class A1>
	BinParserListener * binParserListenerMethodArg1( C * _self, M _method, A1 & _a1 )
	{
		return new BinParserListenerMethodArg1<C,M,A1>(_self, _method, _a1);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class CA1>
	class BinParserListenerMethodCArg1
		: public BinParserListener
	{
	public:
		BinParserListenerMethodCArg1( C * _self, M _method, const CA1 & _ca1 )
			: m_self(_self)
			, m_method(_method)
			, m_ca1(_ca1)
		{
		}

	protected:
		void onElement( BinParser * _parser ) override
		{
			(m_self->*m_method)( _parser, m_ca1 );
		}

		void onEndElement() override
		{
			//Empty
		}

	protected:
		C * m_self;
		M m_method;

		CA1 m_ca1;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class CA1>
	BinParserListener * binParserListenerMethodCArg1( C * _self, M _method, const CA1 & _ca1 )
	{
		return new BinParserListenerMethodCArg1<C,M,CA1>(_self, _method, _ca1);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class A1, class CA2>
	class BinParserListenerMethodArg2
		: public BinParserListener
	{
	public:
		BinParserListenerMethodArg2( C * _self, M _method, A1 & _a1, const CA2 & _ca2 )
			: m_self(_self)
			, m_method(_method)
			, m_a1(_a1)
			, m_ca2(_ca2)
		{
		}

	protected:
		void onElement( BinParser * _parser ) override
		{
			(m_self->*m_method)( _parser, m_a1, m_ca2 );
		}

		void onEndElement() override
		{
			//Empty
		}

	protected:
		C * m_self;
		M m_method;

		A1 & m_a1;

		CA2 m_ca2;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class A1, class CA2>
	BinParserListener * binParserListenerMethodArg2( C * _self, M _method, A1 & _a1, const CA2 & _ca2 )
	{
		return new BinParserListenerMethodArg2<C,M,A1,CA2>(_self, _method, _a1, _ca2);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class CA1, class CA2>
	class BinParserListenerMethodCArg2
		: public BinParserListener
	{
	public:
		BinParserListenerMethodCArg2( C * _self, M _method, const CA1 & _ca1, const CA2 & _ca2 )
			: m_self(_self)
			, m_method(_method)
			, m_ca1(_ca1)
			, m_ca2(_ca2)
		{
		}

	protected:
		void onElement( BinParser * _parser ) override
		{
			(m_self->*m_method)( _parser, m_ca1, m_ca2 );
		}

		void onEndElement() override
		{
			//Empty
		}

	protected:
		C * m_self;
		M m_method;

		CA1 m_ca1;
		CA2 m_ca2;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class CA1, class CA2>
	BinParserListener * binParserListenerMethodCArg2( C * _self, M _method, const CA1 & _ca1, const CA2 & _ca2 )
	{
		return new BinParserListenerMethodCArg2<C,M,CA1,CA2>(_self, _method, _ca1, _ca2);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class E>
	class BinParserListenerMethodEnd
		: public BinParserListener
	{
	public:
		BinParserListenerMethodEnd( C * _self, M _method, E _end )
			: m_self(_self)
			, m_method(_method)
			, m_end(_end)
		{
		}

	protected:
		void onElement( BinParser * _parser ) override
		{
			(m_self->*m_method)( _parser );
		}

		void onEndElement() override
		{
			(m_self->*m_end)();
		}

	protected:
		C * m_self;
		M m_method;
		E m_end;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class E>
	BinParserListener * binParserListenerMethodEnd( C * _self, M _method, E _end )
	{
		return new BinParserListenerMethodEnd<C,M,E>(_self, _method, _end);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class F, class A1>
	class BinParserListenerFunction1
		: public BinParserListener
	{
	public:
		BinParserListenerFunction1( F _func, A1 & _a1 )
			: m_func(_func)
			, m_a1(_a1)
		{
		}

	protected:
		void onElement( BinParser * _parser ) override
		{
			(*m_func)( _parser, m_a1 );
		}

		void onEndElement() override
		{
			//Empty
		}

	protected:
		F m_func;
		A1 & m_a1;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class F, class A1>
	BinParserListener * binParserListenerFunction1( F _func, A1 & _a1 )
	{
		return new BinParserListenerFunction1<F,A1>(_func, _a1);
	}
}

#	define BIN_SWITCH_ID( element )\
	for( BinParser * xmlengine_element = element; xmlengine_element; xmlengine_element = 0 )\
	switch( xmlengine_element->getElementId() )

#	define BIN_CASE_NODE( node )\
	break; case node::id:

#	define BIN_SKIP()\
	break

#	define BIN_FOR_EACH_ATTRIBUTES()\
	while( size_t xmlengine_attribute_id = xmlengine_element->readAttributeId() )\
	switch( xmlengine_attribute_id )

#	define BIN_CASE_ATTRIBUTE( attribute, value )\
	break; case attribute::id: xmlengine_element->readAttribute<attribute::Type>( value );

#	define BIN_CASE_ATTRIBUTE_METHOD( attribute, method )\
	break; case attribute::id: xmlengine_element->readValueMethod<attribute::Type>( this, method );

#	define BIN_CASE_ATTRIBUTE_METHOD_PTR( attribute, ptr, method )\
	break; case attribute::id: xmlengine_element->readValueMethod<attribute::Type>( ptr, method );

#	define BIN_CASE_ATTRIBUTE_METHOD_CHECK( attribute, method )\
	break; case attribute::id: if( xmlengine_element->readValueMethodCheck<attribute::Type>( this, method ) == false ) xmlengine_element->stop()


#	define BIN_CASE_ATTRIBUTE_METHODT( attribute, method, type )\
	break; case attribute::id: xmlengine_element->readValueMethodT<attribute::Type, type>( this, method );

#	define BIN_CASE_ATTRIBUTE_METHOD_IF( attribute, method1, method2 )\
	break; case attribute::id: xmlengine_element->readValueMethodIf( this, method1, method2 );

#	define BIN_CASE_ATTRIBUTE_METHOD_ARG1( attribute, method, arg1 )\
	break; case attribute::id: xmlengine_element->readValueMethodArg1<attribute::Type>( this, method, arg1 );

#	define BIN_CASE_ATTRIBUTE_METHOD_CARG1( attribute, method, carg1 )\
	break; case attribute::id: xmlengine_element->readValueMethodCArg1<attribute::Type>( this, method, carg1 );

#	define BIN_CASE_ATTRIBUTE_FUNCTION_ARG1( attribute, func, arg1 )\
	break; case attribute::id: xmlengine_element->readValueFuncArg1<attribute::Type>( func, arg1 );

#	define BIN_CASE_NODE_PARSE( node, self )\
	break; case node::id: BIN_PARSE( self );

#	define BIN_CASE_NODE_PARSE_METHOD( node, self, method )\
	break; case node::id: BIN_PARSE_METHOD( self, method );

#	define BIN_CASE_NODE_PARSE_METHOD_END( node, self, method, end )\
	break; case node::id: BIN_PARSE_METHOD_END( self, method, end );



#	define BIN_CASE_NODE_PARSE_METHOD_ARG1( node, self, method, arg1 )\
	break; case node::id: BIN_PARSE_METHOD_ARG1( xmlengine_element, self, method, arg1 );

#	define BIN_PARSE_ELEMENT( element, self )\
	do{ BinParserListener * listener = binParserListenerLoadable( self ); element->addListener( listener ); } while(false)

#	define BIN_PARSE_ELEMENT_METHOD( element, self, method )\
	do{ BinParserListener * listener = binParserListenerMethod( self, method ); element->addListener( listener ); } while(false)

#	define BIN_PARSE_ELEMENT_METHOD_END( element, self, method, end )\
	do{ BinParserListener * listener = binParserListenerMethodEnd( self, method, end ); element->addListener( listener ); } while(false)

#	define BIN_PARSE_ELEMENT_METHOD_ARG1( element, self, method, arg1 )\
	do{ BinParserListener * listener = binParserListenerMethodArg1( self, method, arg1 ); element->addListener( listener ); } while(false)

#	define BIN_PARSE_ELEMENT_METHOD_CARG1( element, self, method, carg1 )\
	do{ BinParserListener * listener = binParserListenerMethodCArg1( self, method, carg1 ); element->addListener( listener ); } while(false)

#	define BIN_PARSE_ELEMENT_METHOD_ARG2( element, self, method, arg1, carg2 )\
	do{ BinParserListener * listener = binParserListenerMethodArg2( self, method, arg1, carg2 ); element->addListener( listener ); } while(false)

#	define BIN_PARSE_ELEMENT_METHOD_CARG2( element, self, method, carg1, carg2 )\
	do{ BinParserListener * listener = binParserListenerMethodCArg2( self, method, carg1, carg2 ); element->addListener( listener ); } while(false)

#	define BIN_PARSE_ELEMENT_FUNCTION_ARG1( element, func, arg1 )\
	do{ BinParserListener * listener = binParserListenerFunction1( func, arg1 ); element->addListener( listener ); } while(false)

#	define BIN_PARSE_METHOD( self, method )\
	BIN_PARSE_ELEMENT_METHOD( xmlengine_element, self, method )

#	define BIN_PARSE_METHOD_END( self, method, end )\
	BIN_PARSE_ELEMENT_METHOD_END( xmlengine_element, self, method, end )

#	define BIN_PARSE_METHOD_ARG1( self, method, arg1 )\
	BIN_PARSE_ELEMENT_METHOD_ARG1( xmlengine_element, self, method, arg1 )

#	define BIN_PARSE_METHOD_CARG1( self, method, carg1 )\
	BIN_PARSE_ELEMENT_METHOD_CARG1( xmlengine_element, self, method, carg1 )

#	define BIN_PARSE_METHOD_ARG2( self, method, arg1, carg2 )\
	BIN_PARSE_ELEMENT_METHOD_ARG2( xmlengine_element, self, method, arg1, carg2 )

#	define BIN_PARSE_METHOD_CARG2( self, method, carg1, carg2 )\
	BIN_PARSE_ELEMENT_METHOD_CARG2( xmlengine_element, self, method, carg1, carg2 )


#	define BIN_PARSE( self )\
	BIN_PARSE_ELEMENT( xmlengine_element, self )