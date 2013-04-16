#	include "Kernel/Colorable.h"

#	include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Colorable::Colorable()
		: m_invalidateColor(false)
		, m_localTransparent(false)
		, m_personalTransparent(false)
		, m_colorPersonal(1.0f, 1.0f, 1.0f, 1.0f)
		, m_colorLocal(1.0f, 1.0f, 1.0f, 1.0f)
		, m_colorWorld(1.0f, 1.0f, 1.0f, 1.0f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setPersonalColor( const ColourValue& _color )
	{
		if( m_colorPersonal == _color )
		{
			return;
		}

		m_colorPersonal = _color;

        this->_setPersonalColor( _color );

		this->invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setPersonalAlpha( float _alpha )
	{
		if( fabsf(m_colorPersonal.getA() - _alpha) < 0.00001f )
		{ 
			return;
		}

		m_colorPersonal.setA( _alpha );

        this->_setPersonalAlpha( _alpha );

		this->invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setLocalColor( const ColourValue& _color )
	{
		if( m_colorLocal == _color )
		{
			return;
		}

		m_colorLocal = _color;

		this->invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setLocalAlpha( float _alpha )
	{
		if( fabsf(m_colorLocal.getA() - _alpha) < 0.00001f )
		{
			return;
		}

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

		m_localTransparent = m_colorLocal.getA() < 0.0001f;
		m_personalTransparent = m_colorPersonal.getA() < 0.0001f;

		this->_invalidateColor();
	}
    //////////////////////////////////////////////////////////////////////////
    void Colorable::_setPersonalColor( const ColourValue& _color )
    {
        (void)_color;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::_setPersonalAlpha( float _alpha )
    {
        (void)_alpha;
        //Empty
    }
	//////////////////////////////////////////////////////////////////////////
	void Colorable::_invalidateColor()
	{
		//Empty
	}
}

