#	include "Kernel/Colorable.h"

#	include "Math/utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Colorable::Colorable()
		: m_invalidateColor(false)
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
		if( mt::equal_f_f( m_colorPersonal.getA(), _alpha) == true )
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
	void Colorable::setLocalColorR( float _value )
	{
		if( mt::equal_f_f( m_colorLocal.getR(), _value ) == true )
		{
			return;
		}

		m_colorLocal.setR( _value );

		this->invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setLocalColorG( float _value )
	{
		if( mt::equal_f_f( m_colorLocal.getG(), _value ) == true )
		{
			return;
		}

		m_colorLocal.setG( _value );

		this->invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setLocalColorB( float _value )
	{
		if( mt::equal_f_f( m_colorLocal.getB(), _value ) == true )
		{
			return;
		}

		m_colorLocal.setB( _value );

		this->invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setLocalColorRGB( float _r, float _g, float _b )
	{ 
		m_colorLocal.setR( _r );
		m_colorLocal.setG( _g );
		m_colorLocal.setB( _b );

		this->invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::setLocalAlpha( float _value )
	{
		if( mt::equal_f_f( m_colorLocal.getA(), _value ) == true )
		{
			return;
		}

		m_colorLocal.setA( _value );

		this->invalidateColor();
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue& Colorable::updateRelationColor( const ColourValue& _parentColor ) const
	{
		m_invalidateColor = false;

		m_colorRelation = m_colorLocal;
		m_colorRelation *= _parentColor;

		return m_colorRelation;
	}
	//////////////////////////////////////////////////////////////////////////
	void Colorable::invalidateColor()
	{
		m_invalidateColor = true;

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

