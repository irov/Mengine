#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	typedef uint32 RGBA;
	typedef uint32 ARGB;
	typedef uint32 ABGR;
	typedef uint32 BGRA;

	class ColourValue
	{
	public:
		ColourValue() 
			: m_r(1.f)
			, m_g(1.f)
			, m_b(1.f)
			, m_a(1.f)
			, m_argb(0xFFFFFFFF)
			, m_invalidateARGB(false)        
			, m_identity(true)
		{ 
		}

		explicit ColourValue( float _r, float _g, float _b, float _a ) 
			: m_r(_r)
			, m_g(_g)
			, m_b(_b)
			, m_a(_a)
			, m_invalidateARGB(true)
			, m_identity(false)
		{ 
		}

		explicit ColourValue( ARGB _argb )
		{
			this->setAsARGB( _argb );
		}

		ColourValue( const ColourValue & _copy )
			: m_r(_copy.m_r)
			, m_g(_copy.m_g)
			, m_b(_copy.m_b)
			, m_a(_copy.m_a)
			, m_argb(_copy.m_argb)        
			, m_invalidateARGB(_copy.m_invalidateARGB)
			, m_identity(_copy.m_identity)
		{
		}

		const ColourValue & operator = ( const ColourValue & _other )
		{
			m_r = _other.m_r;
			m_g = _other.m_g;
			m_b = _other.m_b;
			m_a = _other.m_a;

			m_invalidateARGB = _other.m_invalidateARGB;
			m_argb = _other.m_argb;
			m_identity = _other.m_identity;

			return *this;
		}				

	public:
		void setARGB( float _a, float _r, float _g, float _b );

	public:
		void setAsARGB( const ARGB _val );
		inline ARGB getAsARGB() const;

	public:
		bool operator == ( const ColourValue & _rhs ) const
		{
			ARGB self_argb = this->getAsARGB();
			ARGB other_argb = _rhs.getAsARGB();

			return self_argb == other_argb;
		}

		bool operator != ( const ColourValue & _rhs ) const
		{
			return !(*this == _rhs);
		}

		inline void operator *= ( const ColourValue & _rhs )
		{
			if( _rhs.m_identity == true )
			{
				return;
			}

			m_a *= _rhs.m_a;
			m_r *= _rhs.m_r;
			m_g *= _rhs.m_g;
			m_b *= _rhs.m_b;


			this->invalidate();
		}

		inline ColourValue operator + ( const ColourValue& _rkVector ) const
		{
			ColourValue value;

			value.m_a = m_a + _rkVector.m_a;
			value.m_r = m_r + _rkVector.m_r;
			value.m_g = m_g + _rkVector.m_g;
			value.m_b = m_b + _rkVector.m_b;			

			value.invalidate();

			return value;
		}

		inline ColourValue operator - ( const ColourValue & _rkVector ) const
		{
			ColourValue value;

			value.m_a = m_a - _rkVector.m_a;
			value.m_r = m_r - _rkVector.m_r;
			value.m_g = m_g - _rkVector.m_g;
			value.m_b = m_b - _rkVector.m_b;			

			value.invalidate();

			return value;
		}

		inline ColourValue operator * ( const float _fScalar ) const
		{
			ColourValue value;

			value.m_a = m_a * _fScalar;
			value.m_r = m_r * _fScalar;
			value.m_g = m_g * _fScalar;
			value.m_b = m_b * _fScalar;			

			value.invalidate();

			return value;
		}

		inline ColourValue operator * ( const ColourValue & _rhs ) const
		{
			if( m_identity == true )
			{
				return _rhs;
			}

			ColourValue value;

			value.m_a = m_a * _rhs.m_a;
			value.m_r = m_r * _rhs.m_r;
			value.m_g = m_g * _rhs.m_g;
			value.m_b = m_b * _rhs.m_b;			

			value.invalidate();

			return value;
		}

	public:
		void setA( const float _a );
		inline float getA() const;

		void setR( const float _r );
		inline float getR() const;

		void setG( const float _g );
		inline float getG() const;

		void setB( const float _b );
		inline float getB() const;

	public:
		inline bool isIdentity() const;

	protected:		
		inline void invalidate() const;
		void updateARGB() const;

	protected:
		float m_a;
		float m_r;
		float m_g;
		float m_b;

		mutable ARGB m_argb;

		mutable bool m_invalidateARGB;
		mutable bool m_identity;
	};
	//////////////////////////////////////////////////////////////////////////
	inline float ColourValue::getA() const
	{
		return m_a;
	}
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
	inline bool ColourValue::isIdentity() const
	{
		return m_identity;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void ColourValue::invalidate() const
	{
		m_identity = false;
		m_invalidateARGB = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline ARGB ColourValue::getAsARGB() const
	{
		if( m_invalidateARGB == true )
		{
			this->updateARGB();
		}

		return m_argb;
	}
} // namespace
