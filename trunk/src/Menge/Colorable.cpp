#	include "Colorable.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Colorable::Colorable()
		: m_invalidateColor(false)
		, m_colorLocal( 1.0f, 1.0f, 1.0f, 1.0f )
		, m_colorWorld( 1.0f, 1.0f, 1.0f, 1.0f )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setLocalColor( const ColourValue& _color )
	{
		m_colorLocal = _color;

		invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setLocalAlpha( float _alpha )
	{
		m_colorLocal.setA( _alpha );

		setLocalColor( m_colorLocal );
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue& Colorable::updateRelationColor( const ColourValue& _parentColor ) const
	{
		m_invalidateColor = false;

		m_colorWorld = m_colorLocal;
		m_colorWorld *= _parentColor;

		return m_colorWorld;
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::invalidateColor()
	{
		m_invalidateColor = true;

		this->_invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::_invalidateColor()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Color", "Value", &Colorable::setLocalColor );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Alpha", "Value", &Colorable::setLocalAlpha );
		}
	}
}

