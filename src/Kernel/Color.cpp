#include "Color.h"

#include "ColorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    void Color::updateRGBA_() const
    {
        m_argb = Helper::makeRGBAF( m_r, m_g, m_b, m_a );

        if( m_argb == Detail::COLOR_IDENTITY_VALUE )
        {
            m_invalidateARGB = Detail::COLOR_INVALIDATE_IDENTITY;
        }
        else
        {
            m_invalidateARGB = Detail::COLOR_INVALIDATE_FALSE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Color::setRGBA4( const float * _buff )
    {
        m_r = _buff[0];
        m_g = _buff[1];
        m_b = _buff[2];
        m_a = _buff[3];

        this->invalidate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Color::setRGBA( float _r, float _g, float _b, float _a )
    {
        m_r = _r;
        m_g = _g;
        m_b = _b;
        m_a = _a;

        this->invalidate();
    }

    //////////////////////////////////////////////////////////////////////////
    void Color::setRGB( float _r, float _g, float _b )
    {
        m_r = _r;
        m_g = _g;
        m_b = _b;

        this->invalidate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Color::multiplyRGBA( float _r, float _g, float _b, float _a )
    {
        m_r *= _r;
        m_g *= _g;
        m_b *= _b;
        m_a *= _a;

        this->invalidate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Color::premultiplyRGB()
    {
        m_r *= m_a;
        m_g *= m_a;
        m_b *= m_a;

        this->invalidate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Color::clear()
    {
        m_r = 1.f;
        m_g = 1.f;
        m_b = 1.f;
        m_a = 1.f;

        m_argb = Detail::COLOR_IDENTITY_VALUE;
        m_invalidateARGB = Detail::COLOR_INVALIDATE_IDENTITY;
    }
    //////////////////////////////////////////////////////////////////////////
    void Color::setA( float _a )
    {
        m_a = _a;

        this->invalidate();
    }
    //////////////////////////////////////////////////////////////////////////
    float * Color::buff()
    {
        this->invalidate();

        return &m_r;
    }
    //////////////////////////////////////////////////////////////////////////
    const float * Color::buff() const
    {
        return &m_r;
    }
    //////////////////////////////////////////////////////////////////////////
    void Color::setR( float _r )
    {
        m_r = _r;

        this->invalidate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Color::setG( float _g )
    {
        m_g = _g;

        this->invalidate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Color::setB( float _b )
    {
        m_b = _b;

        this->invalidate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Color::setR8( uint8_t _r )
    {
        float r = Helper::makeColorChannel8( _r );

        this->setR( r );
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t Color::getR8() const
    {
        uint8_t r8 = Helper::makeColorChannelF( m_r );

        return r8;
    }
    //////////////////////////////////////////////////////////////////////////
    void Color::setG8( uint8_t _g )
    {
        float g = Helper::makeColorChannel8( _g );

        this->setR( g );
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t Color::getG8() const
    {
        uint8_t g8 = Helper::makeColorChannelF( m_g );

        return g8;
    }
    //////////////////////////////////////////////////////////////////////////
    void Color::setB8( uint8_t _b )
    {
        float b = Helper::makeColorChannel8( _b );

        this->setR( b );
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t Color::getB8() const
    {
        uint8_t b8 = Helper::makeColorChannelF( m_b );

        return b8;
    }
    //////////////////////////////////////////////////////////////////////////
    void Color::setA8( uint8_t _a )
    {
        float a = Helper::makeColorChannel8( _a );

        this->setR( a );
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t Color::getA8() const
    {
        uint8_t a8 = Helper::makeColorChannelF( m_a );

        return a8;
    }
    //////////////////////////////////////////////////////////////////////////
}