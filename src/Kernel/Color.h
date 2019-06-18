#pragma once

#include "Config/Typedef.h"

#include "math/utils.h"

namespace Mengine
{
    typedef uint32_t ColorValue_ARGB;

    namespace Helper
    {
        ColorValue_ARGB makeARGB( float _r, float _g, float _b, float _a );
    }

    namespace Detail
    {
        const uint32_t COLOR_INVALIDATE_IDENTITY = 0;
        const uint32_t COLOR_INVALIDATE_FALSE = 1;
        const uint32_t COLOR_INVALIDATE_TRUE = 2;
        const uint32_t COLOR_IDENTITY_VALUE = 0xFFFFFFFF;
    }

    class Color
    {
    public:
        Color() noexcept
            : m_r( 1.f )
            , m_g( 1.f )
            , m_b( 1.f )
            , m_a( 1.f )
            , m_argb( Detail::COLOR_IDENTITY_VALUE )
            , m_invalidateARGB( Detail::COLOR_INVALIDATE_IDENTITY )
        {
        }

        explicit Color( float _r, float _g, float _b, float _a )
            : m_r( _r )
            , m_g( _g )
            , m_b( _b )
            , m_a( _a )
            , m_argb( Detail::COLOR_IDENTITY_VALUE )
            , m_invalidateARGB( Detail::COLOR_INVALIDATE_TRUE )
        {
        }

        explicit Color( ColorValue_ARGB _argb )
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
        void setAsARGB( ColorValue_ARGB _val );
        inline ColorValue_ARGB getAsARGB() const;

    public:
        bool operator == ( const Color & _rhs ) const
        {
            ColorValue_ARGB self_argb = this->getAsARGB();
            ColorValue_ARGB other_argb = _rhs.getAsARGB();

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
            if( _rhs.getInvalidateRGBA_() == Detail::COLOR_INVALIDATE_IDENTITY )
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

        inline Color operator + ( const Color& _color ) const
        {
            float r = m_r + _color.m_r;
            float g = m_g + _color.m_g;
            float b = m_b + _color.m_b;
            float a = m_a + _color.m_a;

            return Color( r, g, b, a );
        }

        inline Color operator - ( const Color & _color ) const
        {
            float r = m_r - _color.m_r;
            float g = m_g - _color.m_g;
            float b = m_b - _color.m_b;
            float a = m_a - _color.m_a;

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
            if( _rhs.getInvalidateRGBA_() == Detail::COLOR_INVALIDATE_IDENTITY )
            {
                return *this;
            }

            if( this->getInvalidateRGBA_() == Detail::COLOR_INVALIDATE_IDENTITY )
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
        float * buff();
        const float * buff() const;

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
        inline uint32_t getInvalidateRGBA_() const;

    protected:
        float m_r;
        float m_g;
        float m_b;
        float m_a;

        mutable ColorValue_ARGB m_argb;

        mutable uint32_t m_invalidateARGB;
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
        uint32_t invalidateARGB = this->getInvalidateRGBA_();

        return invalidateARGB == Detail::COLOR_INVALIDATE_IDENTITY;
    }
    //////////////////////////////////////////////////////////////////////////
    inline uint32_t Color::getInvalidateRGBA_() const
    {
        if( m_invalidateARGB == Detail::COLOR_INVALIDATE_TRUE )
        {
            this->updateRGBA_();
        }

        return m_invalidateARGB;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void Color::invalidate() const
    {
        m_invalidateARGB = Detail::COLOR_INVALIDATE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    inline ColorValue_ARGB Color::getAsARGB() const
    {
        if( m_invalidateARGB == Detail::COLOR_INVALIDATE_TRUE )
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
}
