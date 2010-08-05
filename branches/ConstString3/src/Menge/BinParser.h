#	pragma once

#	include "Utils/Core/ConstString.h"
#	include "Utils/Core/Resolution.h"
#	include "Utils/Core/ColourValue.h"
#	include "Utils/Core/Viewport.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include "Math/vec4.h"
#	include "Math/mat3.h"

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
		virtual void onElement( BinParser * _parser ) = 0;
	};

	void operator >> ( ArchiveRead & ar, ConstString & _value );
	void operator >> ( ArchiveRead & ar, Resolution & _value );
	void operator >> ( ArchiveRead & ar, ColourValue & _value );
	void operator >> ( ArchiveRead & ar, Viewport & _value );

	void operator >> ( ArchiveRead & ar, mt::vec2f & _value );
	void operator >> ( ArchiveRead & ar, mt::vec3f & _value );
	void operator >> ( ArchiveRead & ar, mt::vec4f & _value );
	void operator >> ( ArchiveRead & ar, mt::mat3f & _value );

	class BinParserException
		: public std::exception
	{
	public:
		BinParserException( const std::string & _reason );
		~BinParserException() throw() { }

	protected:
		const char * what() const throw() override;
		
	protected:
		std::string m_reason;
	};

	class BinParser
	{
	public:
		BinParser( Archive::const_iterator _begin, Archive::const_iterator _end );

	public:
		bool run( BinParserListener * _listener );

	public:
		void addListener( BinParserListener * _listener );

	public:
		template<class T>
		void readValue( T & _value )
		{
			m_reader >> _value;

			m_debugNeedReadValue = false;
		}

		template<class T, class C, class M>
		void readValueMethod( C * _self, M _method )
		{
			T value;
			this->readValue( value );

			(_self->*_method)( value );
		}

		template<class T1, class T2, class C, class M>
		void readValueMethodT( C * _self, M _method )
		{
			T1 value;
			this->readValue( value );

			(_self->*_method)( static_cast<T2>(value) );
		}


		template<class C, class M>
		void readValueMethodIf( C * _self, M _method1, M _method2 )
		{
			bool value;
			this->readValue( value );

			value ? (_self->*_method1)() : (_self->*_method2)();
		}

		template<class T, class F, class A1>
		void readValueFuncArg1( F _func, A1 & _arg1 )
		{
			T value;
			this->readValue( value );

			(*_func)( value, _arg1 );
		}
	
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

		template<class T>
		void readAttribute( T & _value )
		{
			this->readValue( _value );
		}

	public:
		inline size_t getElementId() const;

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

		size_t m_elementId;

		bool m_debugNeedReadValue;
	};
	//////////////////////////////////////////////////////////////////////////
	inline size_t BinParser::getElementId() const
	{
		return m_elementId;
	}
	//////////////////////////////////////////////////////////////////////////
	inline int BinParser::getAttributeCount() const
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

	protected:
		Loadable * m_self;
	};
	//////////////////////////////////////////////////////////////////////////
	BinParserListener * binParserListenerLoadable( Loadable * _self );
	//////////////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class A1>
	BinParserListener * binParserListenerMethod1( C * _self, M _method, A1 & _a1 )
	{
		return new BinParserListenerMethod1<C,M,A1>(_self, _method, _a1);
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

#	define BIN_CASE_ATTRIBUTE_METHODT( attribute, method, type )\
	break; case attribute::id: xmlengine_element->readValueMethodT<attribute::Type, type>( this, method );

#	define BIN_CASE_ATTRIBUTE_METHOD_IF( attribute, method1, method2 )\
	break; case attribute::id: xmlengine_element->readValueMethodIf( this, method1, method2 );

#	define BIN_CASE_ATTRIBUTE_FUNCTION_ARG1( attribute, func, arg1 )\
	break; case attribute::id: xmlengine_element->readValueFuncArg1<attribute::Type>( func, arg1 );

#	define BIN_CASE_NODE_PARSE( node, self )\
	break; case node::id: BIN_PARSE( self );

#	define BIN_CASE_NODE_PARSE_METHOD( node, self, method )\
	break; case node::id: BIN_PARSE_METHOD( self, method );

#	define BIN_CASE_NODE_PARSE_METHOD_ARG1( node, self, method, arg1 )\
	break; case node::id: BIN_PARSE_METHOD_ARG1( xmlengine_element, self, method, arg1 );

#	define BIN_PARSE_ELEMENT( element, self )\
	do{ BinParserListener * listener = binParserListenerLoadable( self ); element->addListener( listener ); } while(false)

#	define BIN_PARSE_ELEMENT_METHOD( element, self, method )\
	do{ BinParserListener * listener = binParserListenerMethod( self, method ); element->addListener( listener ); } while(false)

#	define BIN_PARSE_ELEMENT_METHOD_ARG1( element, self, method, arg1 )\
	do{ BinParserListener * listener = binParserListenerMethod1( self, method, arg1 ); element->addListener( listener ); } while(false)

#	define BIN_PARSE_ELEMENT_FUNCTION_ARG1( element, func, arg1 )\
	do{ BinParserListener * listener = binParserListenerFunction1( func, arg1 ); element->addListener( listener ); } while(false)

#	define BIN_PARSE_METHOD( self, method )\
	BIN_PARSE_ELEMENT_METHOD( xmlengine_element, self, method )

#	define BIN_PARSE_METHOD_ARG1( self, method, arg1 )\
	BIN_PARSE_ELEMENT_METHOD_ARG1( xmlengine_element, self, method, arg1 )

#	define BIN_PARSE( self )\
	BIN_PARSE_ELEMENT( xmlengine_element, self )