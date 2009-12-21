#	pragma once

#	include "XmlElementListener.h"
#	include "XmlElementValueListener.h"

class XmlElement;

namespace XmlParserElement
{
	bool element_compare_title( XmlElement * _element, const Menge::TChar * _title );
	const Menge::TChar * element_get_title( XmlElement * _element );
	void element_begin_attributes( XmlElement * _element );
	bool element_valid_attributes( XmlElement * _element );
	void element_next_attributes( XmlElement * _element );
	bool element_compare_attribute_key( XmlElement * _element, const Menge::TChar * _key );
	const Menge::TChar * element_get_attribute_value( XmlElement * _element );
	void element_push_listener_impl( XmlElement * _element, XmlElementListener * _listener );
	void element_set_listener_value_impl( XmlElement * _element, XmlElementValueListener * _listener );

	template<class C, class M>
	static void element_push_listener( XmlElement * _element, C * _self, M _method )
	{
		XmlElementListener * listener = makeXmlElementListener( _self, _method );
		element_push_listener_impl( _element, listener );
	}

	template<class C, class M, class A1>
	static void element_push_listener_a( XmlElement * _element, C * _self, M _method, const A1 & _a1 )
	{
		XmlElementListener * listener = makeXmlElementListenerA( _self, _method, _a1 );
		element_push_listener_impl( _element, listener );
	}

	template<class C, class M>
	void element_set_listener_value( XmlElement * _element, C * _self, M _method  )
	{
		XmlElementValueListener * listener = makeXmlElementValueListener( _self, _method );
		element_set_listener_value_impl( _element, listener );
	}
}
