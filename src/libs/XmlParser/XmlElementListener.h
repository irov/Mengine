#	pragma once

class XmlElement;

class XmlElementListener
{
public:
	XmlElementListener();
	virtual ~XmlElementListener();

public:
	void incref();
	bool decref();

public:
	virtual void parseXML( XmlElement * _xml ) = 0;

protected:
	unsigned int m_reference;
};

template<class C, class M>
class XmlElementListenerMethod
	: public XmlElementListener
{
public:
	XmlElementListenerMethod( C * _self, M _method )
		: m_self(_self)
		, m_method( _method )
	{
	}

public:
	void parseXML( XmlElement * _xml ) override
	{
		(m_self->*m_method)(_xml);
	}

protected:
	C * m_self;
	M m_method;
};

template<class F>
class XmlElementListenerFunction
	: public XmlElementListener
{
public:
	XmlElementListenerFunction( F _function )
		: m_function(_function)
	{
	}

public:
	void parseXML( XmlElement * _xml ) override
	{
		(*m_function)( _xml );
	}

protected:
	F m_function;
};

template<class C, class M>
XmlElementListenerMethod<C,M> * makeXmlElementListener( C * _self, M _method )
{
	return new XmlElementListenerMethod<C,M>( _self, _method );
}
