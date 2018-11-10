#pragma once

#include "Config/Typedef.h"

#include "math/utils.h"

namespace Mengine
{
    typedef uint32_t ColourValue_ARGB;

    namespace Helper
    {
        ColourValue_ARGB makeARGB( float _r, float _g, float _b, float _a );
    }

    const uint8_t COLOUR_VALUE_INVALIDATE_IDENTITY = 0;
    const uint8_t COLOUR_VALUE_INVALIDATE_FALSE = 1;
    const uint8_t COLOUR_VALUE_INVALIDATE_TRUE = 2;

    class Color
    {
    public:
        Color() noexcept
            : m_r( 1.f )
            , m_g( 1.f )
            , m_b( 1.f )
            , m_a( 1.f )
            , m_argb( 0xFFFFFFFF )
            , m_invalidateARGB( COLOUR_VALUE_INVALIDATE_IDENTITY )
        {
        }

        explicit Color( float _r, float _g, float _b, float _a )
            : m_r( _r )
            , m_g( _g )
            , m_b( _b )
            , m_a( _a )
            , m_argb( 0xFFFFFFFF )
            , m_invalidateARGB( COLOUR_VALUE_INVALIDATE_TRUE )
        {
        }

        explicit Color( ColourValue_ARGB _argb )
        {
            this->setAsARGB( _argb );
        }

        Color( const Color & _copy )
            : m_r( _copy.m_r )
            , m_g( _copy.m_g )
            , m_b( _copy.m_b )
            , m_a( _copy.m_a )
            , m_argb( _copy.m_argb )
            , m_invalidateARGB( _copy.m_invalidateARGB )
        {
        }

        Color & operator = ( const Color & _other )
        {
            m_r = _other.m_r;
            m_g = _other.m_g;
            m_b = _other.m_b;
            m_a = _other.m_a;

            m_invalidateARGB = _other.m_invalidateARGB;
            m_argb = _other.m_argb;

            return *this;
        }

    public:
        void setRGBA( float _r, float _g, float _b, float _a );
        void setRGB( float _r, float _g, float _b );

    public:
        void multiplyRGBA( float _r, float _g, float _b, float _a );
        void premultiplyRGB();

    public:
        void setAsARGB( ColourValue_ARGB _val );
        inline ColourValue_ARGB getAsARGB() const;

    public:
        bool operator == ( const Color & _rhs ) const
        {
            ColourValue_ARGB self_argb = this->getAsARGB();
            ColourValue_ARGB other_argb = _rhs.getAsARGB();

            return self_argb == other_argb;
        }

        bool operator != ( const Color & _rhs ) const
        {
            return !(*this == _rhs);
        }

        inline Color & operator *= ( const float _f )
        {
            m_r *= _f;
            m_g *= _f;
            m_b *= _f;
            m_a *= _f;

            this->invalidate();

            return *this;
        }

        inline Color & operator *= ( const Color & _rhs )
        {
            if( _rhs.getInvalidateRGBA_() == COLOUR_VALUE_INVALIDATE_IDENTITY )
            {
                return *this;
            }

            m_a *= _rhs.m_a;
            m_r *= _rhs.m_r;
            m_g *= _rhs.m_g;
            m_b *= _rhs.m_b;

            this->invalidate();

            return *this;
        }

        inline Color operator + ( const Color& _colour ) const
        {
            float r = m_r + _colour.m_r;
            float g = m_g + _colour.m_g;
            float b = m_b + _colour.m_b;
            float a = m_a + _colour.m_a;

            return Color( r, g, b, a );
        }

        inline Color operator - ( const Color & _colour ) const
        {
            float r = m_r - _colour.m_r;
            float g = m_g - _colour.m_g;
            float b = m_b - _colour.m_b;
            float a = m_a - _colour.m_a;

            return Color( r, g, b, a );
        }

        inline Color operator * ( float _f ) const
        {
            float r = m_r * _f;
            float g = m_g * _f;
            float b = m_b * _f;
            float a = m_a * _f;

            return Color( r, g, b, a );
        }

        inline Color operator * ( const Color & _rhs ) const
        {
            if( _rhs.getInvalidateRGBA_() == COLOUR_VALUE_INVALIDATE_IDENTITY )
            {
                return *this;
            }

            if( this->getInvalidateRGBA_() == COLOUR_VALUE_INVALIDATE_IDENTITY )
            {
                return _rhs;
            }

            float r = m_r * _rhs.m_r;
            float g = m_g * _rhs.m_g;
            float b = m_b * _rhs.m_b;
            float a = m_a * _rhs.m_a;

            return Color( r, g, b, a );
        }

    public:
        void setR( float _r );
        inline float getR() const;

        void setG( float _g );
        inline float getG() const;

        void setB( float _b );
        inline float getB() const;

        void setA( float _a );
        inline float getA() const;

    public:
        inline static const Color & identity();

    public:
        inline bool isSolid() const;

    public:
        inline bool isIdentity() const;

    protected:
        inline void invalidate() const;

    protected:
        void updateRGBA_() const;
        inline uint8_t getInvalidateRGBA_() const;

    protected:
        float m_r;
        float m_g;
        float m_b;
        float m_a;

        mutable ColourValue_ARGB m_argb;

        mutable uint8_t m_invalidateARGB;
    };
    //////////////////////////////////////////////////////////////////////////
    inline float Color::getR() const
    {
        return m_r;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float Color::getG() const
    {
        return m_g;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float Color::getB() const
    {
        return m_b;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float Color::getA() const
    {
        return m_a;
    }
    //////////////////////////////////////////////////////////////////////////	
    inline bool Color::isSolid() const
    {
        return mt::equal_f_1( m_a );
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool Color::isIdentity() const
    {
        uint8_t invalidateARGB = this->getInvalidateRGBA_();

        return invalidateARGB == COLOUR_VALUE_INVALIDATE_IDENTITY;
    }
    //////////////////////////////////////////////////////////////////////////
    inline uint8_t Color::getInvalidateRGBA_() const
    {
        if( m_invalidateARGB == COLOUR_VALUE_INVALIDATE_TRUE )
        {
            this->updateRGBA_();
        }

        return m_invalidateARGB;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void Color::invalidate() const
    {
        m_invalidateARGB = COLOUR_VALUE_INVALIDATE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    inline ColourValue_ARGB Color::getAsARGB() const
    {
        if( m_invalidateARGB == COLOUR_VALUE_INVALIDATE_TRUE )
        {
            this->updateRGBA_();
        }

        return m_argb;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const Color & Color::identity()
    {
        static Color c;

        return c;
    }
} // namespace
