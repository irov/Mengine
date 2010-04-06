#	pragma once

#	include "XmlParserCheckMethod.h"

class XmlElement;

class XmlElementValueListener
{
public:
	virtual ~XmlElementValueListener(){};

public:
	virtual void call( const Menge::TChar * _value ) = 0;
};

template<class C, class M>
class XmlElementValueListenerMethod
	: public XmlElementValueListener
{
public:
	XmlElementValueListenerMethod( C * _self, M _method )
		: m_self(_self)
		, m_method( _method )
	{
	}

public:
	void call( const Menge::TChar * _value ) override
	{
		XmlParserCheckMethod::check_member( m_self, m_method, _value );
	}

protected:
	C * m_self;
	M m_method;
};

template<class C, class M>
XmlElementValueListenerMethod<C,M> * makeXmlElementValueListener( C * _self, M _method )
{
	return new XmlElementValueListenerMethod<C,M>( _self, _method );
}
