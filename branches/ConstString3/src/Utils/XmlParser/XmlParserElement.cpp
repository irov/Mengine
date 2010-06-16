#	include "XmlParserElement.h"

#	include "XmlElement.h"

namespace XmlParserElement
{
	//////////////////////////////////////////////////////////////////////////
	bool element_compare_title( XmlElement * _element, const Menge::TChar * _title )
	{
		if( _element == 0 )
		{
			return false;
		}

		bool result = _element->compareTitle( _title );
		
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	const Menge::TChar * element_get_title( XmlElement * _element )
	{
		return (_element)?_element->getTitle():0;
	}
	//////////////////////////////////////////////////////////////////////////
	void element_begin_attributes( XmlElement * _element )
	{
		_element->beginAttributes();
	}
	//////////////////////////////////////////////////////////////////////////
	bool element_valid_attributes( XmlElement * _element )
	{
		return _element->validAttributes();
	}
	//////////////////////////////////////////////////////////////////////////
	void element_next_attributes( XmlElement * _element )
	{
		_element->nextAttributes();
	}
	//////////////////////////////////////////////////////////////////////////
	bool element_compare_attribute_key( XmlElement * _element, const Menge::TChar * _key )
	{
		return _element->compareAttributeKey( _key );
	}
	//////////////////////////////////////////////////////////////////////////
	const Menge::TChar * element_get_attribute_value( XmlElement * _element )
	{
		return _element->getAttributeValue();
	}
	//////////////////////////////////////////////////////////////////////////
	void element_push_listener_impl( XmlElement * _element, XmlElementListener * _listener )
	{
		_element->pushListener( _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void element_set_listener_value_impl( XmlElement * _element, XmlElementValueListener * _listener )
	{
		_element->setValueListener( _listener );
	}
}
