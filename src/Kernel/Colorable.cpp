#include "Kernel/Colorable.h"

#include "math/utils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Colorable::Colorable()
        : m_invalidateRelationColor( false )
        , m_personalTransparent( false )
        , m_localTransparent( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Colorable::~Colorable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::setPersonalColor( const Color & _color )
    {
        if( m_personalColor == _color )
        {
            return;
        }

        m_personalColor = _color;

        this->updatePersonalTransparent_();

        this->_setPersonalColor( _color );

        this->invalidateColor();
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::setPersonalAlpha( float _alpha )
    {
        if( mt::equal_f_f( m_personalColor.getA(), _alpha ) == true )
        {
            return;
        }

        m_personalColor.setA( _alpha );

        this->updatePersonalTransparent_();

        this->_setPersonalAlpha( _alpha );

        this->invalidateColor();
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::setLocalColor( const Color & _color )
    {
        if( m_localColor == _color )
        {
            return;
        }

        m_localColor = _color;

        this->updateLocalTransparent_();

        this->invalidateColor();
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::setLocalColorR( float _value )
    {
        if( mt::equal_f_f( m_localColor.getR(), _value ) == true )
        {
            return;
        }

        m_localColor.setR( _value );

        this->invalidateColor();
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::setLocalColorG( float _value )
    {
        if( mt::equal_f_f( m_localColor.getG(), _value ) == true )
        {
            return;
        }

        m_localColor.setG( _value );

        this->invalidateColor();
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::setLocalColorB( float _value )
    {
        if( mt::equal_f_f( m_localColor.getB(), _value ) == true )
        {
            return;
        }

        m_localColor.setB( _value );

        this->invalidateColor();
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::setLocalColorRGB( float _r, float _g, float _b )
    {
        m_localColor.setRGB( _r, _g, _b );

        this->invalidateColor();
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::setLocalColorRGBA( float _r, float _g, float _b, float _a )
    {
        m_localColor.setRGBA( _r, _g, _b, _a );

        this->updateLocalTransparent_();

        this->invalidateColor();
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::setLocalAlpha( float _value )
    {
        if( mt::equal_f_f( m_localColor.getA(), _value ) == true )
        {
            return;
        }

        m_localColor.setA( _value );

        this->updateLocalTransparent_();

        this->invalidateColor();
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & Colorable::updateRelationColor( const Color & _parentColor ) const
    {
        m_invalidateRelationColor = false;

        m_relationColor = m_localColor;
        m_relationColor *= _parentColor;

        return m_relationColor;
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::_setLocalTransparent( bool _transparent )
    {
        MENGINE_UNUSED( _transparent );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::_setPersonalTransparent( bool _transparent )
    {
        MENGINE_UNUSED( _transparent );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::updateLocalTransparent_()
    {
        if( m_localTransparent == false )
        {
            if( m_localColor.getA() < MENGINE_COLOR_MINIMAL_ALPHA )
            {
                m_localTransparent = true;

                this->_setLocalTransparent( true );
            }
        }
        else
        {
            if( m_localColor.getA() > MENGINE_COLOR_MINIMAL_ALPHA )
            {
                m_localTransparent = false;

                this->_setLocalTransparent( false );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::updatePersonalTransparent_()
    {
        if( m_personalTransparent == false )
        {
            if( m_personalColor.getA() < MENGINE_COLOR_MINIMAL_ALPHA )
            {
                m_personalTransparent = true;

                this->_setPersonalTransparent( true );
            }
        }
        else
        {
            if( m_personalColor.getA() > MENGINE_COLOR_MINIMAL_ALPHA )
            {
                m_personalTransparent = false;

                this->_setPersonalTransparent( false );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::invalidateColor() const
    {
        m_invalidateRelationColor = true;

        this->_invalidateColor();
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::_setPersonalColor( const Color & _color )
    {
        MENGINE_UNUSED( _color );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::_setPersonalAlpha( float _alpha )
    {
        MENGINE_UNUSED( _alpha );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Colorable::_invalidateColor() const
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}

