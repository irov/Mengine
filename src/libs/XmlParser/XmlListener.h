#	pragma once

class XmlElement;

class XmlListener
{
public:
	XmlListener();
	virtual ~XmlListener();

public:
	void incref();
	bool decref();

public:
	virtual void parseXML( XmlElement * _xml ) = 0;

protected:
	size_t m_reference;
};

template<class C, class F>
class XmlListenerMethod
	: public XmlListener
{
public:
	XmlListenerMethod( C * _class, F _method )
		: m_class(_class)
		, m_method(_method)
	{
	}

public:
	void parseXML( XmlElement * _xml ) override
	{
		(m_class->*m_method)( _xml );
	}

protected:
	C * m_class;
	F m_method;
};

template<class F>
class XmlListenerFunction
	: public XmlListener
{
public:
	XmlListenerFunction( F _method )
		: m_method(_method)
	{
	}

public:
	void parseXML( XmlElement * _xml ) override
	{
		(*m_method)( _xml );
	}

protected:
	F m_method;
};

template<class C, class F>
XmlListenerMethod<C,F> * xmlListenerMethod( C * _class, F _method )
{
	return new XmlListenerMethod<C,F>( _class, _method );
}

template<class F>
XmlListenerFunction<F> * xmlListenerFunction( F _method )
{
	return new XmlListenerFunction<F>( _method );
}
