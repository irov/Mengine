#	include "Colorable.h"

#	include "BinParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Colorable::Colorable()
		: m_invalidateColor(false)
		, m_fullBlend(false)
		, m_colorPersonal(1.0f, 1.0f, 1.0f, 1.0f)
		, m_colorLocal(1.0f, 1.0f, 1.0f, 1.0f)
		, m_colorWorld(1.0f, 1.0f, 1.0f, 1.0f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setFullBlend( bool _value )
	{
		m_fullBlend = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Color_Value, &Colorable::setLocalColor );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Alpha_Value, &Colorable::setLocalAlpha );//BinNew
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setPersonalColor( const ColourValue& _color )
	{
		m_colorPersonal = _color;

		this->invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setPersonalAlpha( float _alpha )
	{
		m_colorPersonal.setA( _alpha );

		this->invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setLocalColor( const ColourValue& _color )
	{
		m_colorLocal = _color;

		this->invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setLocalAlpha( float _alpha )
	{
		m_colorLocal.setA( _alpha );

		this->invalidateColor();
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
}

