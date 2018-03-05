#	pragma once

#	include "Config/Typedef.h"

#	include "Math/utils.h"

namespace Menge
{
	typedef uint32_t ColourValue_ARGB;

	namespace Helper
	{
		ColourValue_ARGB makeARGB( float _r, float _g, float _b, float _a );
	}

	const uint8_t COLOUR_VALUE_INVALIDATE_IDENTITY = 0;
	const uint8_t COLOUR_VALUE_INVALIDATE_FALSE = 1;
	const uint8_t COLOUR_VALUE_INVALIDATE_TRUE = 2;

	class ColourValue
	{
	public:
		ColourValue()
			: m_r( 1.f )
			, m_g( 1.f )
			, m_b( 1.f )
            , m_a( 1.f )
			, m_argb( 0xFFFFFFFF )
			, m_invalidateARGB( COLOUR_VALUE_INVALIDATE_IDENTITY )
		{
		}

        explicit ColourValue( float _r, float _g, float _b, float _a )
			: m_r( _r )
			, m_g( _g )
			, m_b( _b )
            , m_a( _a )
			, m_argb( 0xFFFFFFFF )
			, m_invalidateARGB( COLOUR_VALUE_INVALIDATE_TRUE )
		{
		}

		explicit ColourValue( ColourValue_ARGB _argb )
		{
			this->setAsARGB( _argb );
		}

		ColourValue( const ColourValue & _copy )
			: m_r( _copy.m_r )
			, m_g( _copy.m_g )
			, m_b( _copy.m_b )
            , m_a( _copy.m_a )
			, m_argb( _copy.m_argb )
			, m_invalidateARGB( _copy.m_invalidateARGB )
		{
		}

		const ColourValue & operator = (const ColourValue & _other)
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
		void setAsARGB( const ColourValue_ARGB _val );
		inline ColourValue_ARGB getAsARGB() const;

	public:
		bool operator == (const ColourValue & _rhs) const
		{
			ColourValue_ARGB self_argb = this->getAsARGB();
			ColourValue_ARGB other_argb = _rhs.getAsARGB();

			return self_argb == other_argb;
		}

		bool operator != (const ColourValue & _rhs) const
		{
			return !(*this == _rhs);
		}

		inline ColourValue & operator *= (const float _f)
		{
			m_r *= _f;
			m_g *= _f;
			m_b *= _f;
            m_a *= _f;

			this->invalidate();

			return *this;
		}

		inline ColourValue & operator *= (const ColourValue & _rhs)
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

		inline ColourValue operator + (const ColourValue& _colour) const
		{			
			float r = m_r + _colour.m_r;
			float g = m_g + _colour.m_g;
			float b = m_b + _colour.m_b;
            float a = m_a + _colour.m_a;

			return ColourValue( r, g, b, a );
		}

		inline ColourValue operator - (const ColourValue & _colour) const
		{	
			float r = m_r - _colour.m_r;
			float g = m_g - _colour.m_g;
			float b = m_b - _colour.m_b;
            float a = m_a - _colour.m_a;

			return ColourValue( r, g, b, a );
		}

		inline ColourValue operator * (const float _f) const
		{			
			float r = m_r * _f;
			float g = m_g * _f;
			float b = m_b * _f;
            float a = m_a * _f;

			return ColourValue( r, g, b, a );
		}

		inline ColourValue operator * (const ColourValue & _rhs) const
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

			return ColourValue( r, g, b, a );
		}

	public:
		void setR( const float _r );
		inline float getR() const;

		void setG( const float _g );
		inline float getG() const;

		void setB( const float _b );
		inline float getB() const;

        void setA( const float _a );
        inline float getA() const;

    public:
        inline static const ColourValue & identity();

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
	inline float ColourValue::getR() const
	{
		return m_r;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float ColourValue::getG() const
	{
		return m_g;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float ColourValue::getB() const
	{
		return m_b;
	}
    //////////////////////////////////////////////////////////////////////////
    inline float ColourValue::getA() const
    {
        return m_a;
    }
    //////////////////////////////////////////////////////////////////////////	
	inline bool ColourValue::isSolid() const
	{
		return mt::equal_f_1( m_a );
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool ColourValue::isIdentity() const
	{
		uint8_t invalidateARGB = this->getInvalidateRGBA_();

		return invalidateARGB == COLOUR_VALUE_INVALIDATE_IDENTITY;
	}
	//////////////////////////////////////////////////////////////////////////
	inline uint8_t ColourValue::getInvalidateRGBA_() const
	{
		if( m_invalidateARGB == COLOUR_VALUE_INVALIDATE_TRUE )
		{
			this->updateRGBA_();
		}

		return m_invalidateARGB;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void ColourValue::invalidate() const
	{
		m_invalidateARGB = COLOUR_VALUE_INVALIDATE_TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	inline ColourValue_ARGB ColourValue::getAsARGB() const
	{
		if( m_invalidateARGB == COLOUR_VALUE_INVALIDATE_TRUE )
		{
			this->updateRGBA_();
		}

		return m_argb;
	}
    //////////////////////////////////////////////////////////////////////////
    inline const ColourValue & ColourValue::identity()
    {
        static ColourValue c;

        return c;
    }
} // namespace
