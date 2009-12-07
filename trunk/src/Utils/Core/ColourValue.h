#	pragma once

#	include <cassert>

#	include "Config/Typedef.h"


namespace Menge
{

	typedef uint32 RGBA;
	typedef uint32 ARGB;
	typedef uint32 ABGR;
	typedef uint32 BGRA;

	//Class representing colour.
	//@remarks
	//Colour is represented as 4 components, each of which is a
	//floating-point value from 0.0 to 1.0.
	//@par
	//The 3 'normal' colour components are red, green and blue, a higher
	//number indicating greater amounts of that component in the colour.
	//The forth component is the 'alpha' value, which represents
	//transparency. In this case, 0.0 is completely transparent and 1.0 is
	//fully opaque.
	class ColourValue
	{
	public:
		static const ColourValue ZERO;
		static const ColourValue Black;
		static const ColourValue White;
		static const ColourValue Red;
		static const ColourValue Green;
		static const ColourValue Blue;

		ColourValue() 
			: r( 1.f )
			, g( 1.f )
			, b( 1.f )
			, a( 1.f )
			, m_argb(0xFFFFFFFF)
			, m_invalidateARGB( false )
			, m_identity( true )
		{ 
		}

		explicit ColourValue( bool _invalidate ) 
			: r( 1.f )
			, g( 1.f )
			, b( 1.f )
			, a( 1.f )
			, m_argb(0xFFFFFFFF)
			, m_invalidateARGB( true )
			, m_identity( false )
		{ 
		}

		explicit ColourValue( float _r, float _g, float _b, float _a ) 
			: r( _r )
			, g( _g )
			, b( _b )
			, a( _a )
			, m_invalidateARGB( true )
			, m_identity( false )
		{ 
		}

		explicit ColourValue( ARGB _argb )
		{
			setAsARGB( _argb );
		}

		ColourValue( const ColourValue& _copy )
			: r( _copy.r )
			, g( _copy.g )
			, b( _copy.b )
			, a( _copy.a )
			, m_invalidateARGB( _copy.m_invalidateARGB )
			, m_argb( _copy.m_argb )
			, m_identity( _copy.m_identity )
		{
		}

		const ColourValue& operator=( const ColourValue& _other )
		{
			r = _other.r;
			g = _other.g;
			b = _other.b;
			a = _other.a;
			m_invalidateARGB = _other.m_invalidateARGB;
			m_argb = _other.m_argb;
			m_identity = _other.m_identity;
			return *this;
		}

		bool operator==(const ColourValue& _rhs) const;
		bool operator!=(const ColourValue& _rhs) const;

		// Retrieves colour as RGBA.
		RGBA getAsRGBA() const;

		// Retrieves colour as ARGB.
		ARGB getAsARGB() const;

		// Retrieves colour as BGRA.
		BGRA getAsBGRA() const;

		// Retrieves colours as ABGR
		ABGR getAsABGR() const;

		// Sets colour as RGBA.
		void setAsRGBA( const RGBA _val );

		// Sets colour as ARGB.
		void setAsARGB( const ARGB _val );

		// Sets colour as BGRA.
		void setAsBGRA( const BGRA _val );

		// Sets colour as ABGR.
		void setAsABGR( const ABGR _val );

		// Clamps colour value to the range [0, 1].
		void saturate()
		{
			if (r < 0)
				r = 0;
			else if (r > 1)
				r = 1;

			if (g < 0)
				g = 0;
			else if (g > 1)
				g = 1;

			if (b < 0)
				b = 0;
			else if (b > 1)
				b = 1;

			if (a < 0)
				a = 0;
			else if (a > 1)
				a = 1;

			m_invalidateARGB = true;
			m_identity = false;
		}

		//As saturate, except that this colour value is unaffected and
		//the saturated colour value is returned as a copy.
		ColourValue saturateCopy() const
		{
			ColourValue ret = *this;
			ret.saturate();
			return ret;
		}

		/// Array accessor operator
		inline float operator [] ( const size_t _i ) const
		{
			assert( _i < 4 );

			return *(&r+_i);
		}

		/// Pointer accessor for direct copying
		inline const float* ptr() const
		{
			return &r;
		}

		// arithmetic operations
		inline ColourValue operator + ( const ColourValue& _rkVector ) const
		{
			ColourValue kSum(false);

			kSum.r = r + _rkVector.r;
			kSum.g = g + _rkVector.g;
			kSum.b = b + _rkVector.b;
			kSum.a = a + _rkVector.a;

			return kSum;
		}

		inline ColourValue operator - ( const ColourValue& _rkVector ) const
		{
			ColourValue kDiff(false);

			kDiff.r = r - _rkVector.r;
			kDiff.g = g - _rkVector.g;
			kDiff.b = b - _rkVector.b;
			kDiff.a = a - _rkVector.a;

			return kDiff;
		}

		inline ColourValue operator * ( const float _fScalar ) const
		{
			ColourValue kProd(false);

			kProd.r = _fScalar*r;
			kProd.g = _fScalar*g;
			kProd.b = _fScalar*b;
			kProd.a = _fScalar*a;

			return kProd;
		}

		inline ColourValue operator * ( const ColourValue& _rhs ) const
		{
			if( m_identity )
			{
				return _rhs;
			}

			ColourValue kProd(false);

			kProd.r = _rhs.r * r;
			kProd.g = _rhs.g * g;
			kProd.b = _rhs.b * b;
			kProd.a = _rhs.a * a;

			return kProd;
		}

		inline ColourValue operator / ( const ColourValue& _rhs ) const
		{
			if( _rhs.m_identity )
			{
				return *this;
			}

			ColourValue kProd(false);

			kProd.r = _rhs.r / r;
			kProd.g = _rhs.g / g;
			kProd.b = _rhs.b / b;
			kProd.a = _rhs.a / a;

			return kProd;
		}

		inline ColourValue operator / ( const float _fScalar ) const
		{
			assert( _fScalar != 0.0 );

			ColourValue kDiv(false);

			float fInv = 1.0f / _fScalar;
			kDiv.r = r * fInv;
			kDiv.g = g * fInv;
			kDiv.b = b * fInv;
			kDiv.a = a * fInv;

			return kDiv;
		}

		inline friend ColourValue operator * ( const float _fScalar, const ColourValue& _rkVector )
		{
			ColourValue kProd(false);

			kProd.r = _fScalar * _rkVector.r;
			kProd.g = _fScalar * _rkVector.g;
			kProd.b = _fScalar * _rkVector.b;
			kProd.a = _fScalar * _rkVector.a;

			return kProd;
		}

		inline void operator *= ( const ColourValue& _rhs )
		{
			if( _rhs.m_identity )
			{
				return;
			}

			r *= _rhs.r;
			g *= _rhs.g;
			b *= _rhs.b;
			a *= _rhs.a;

			invalidate();
		}

		// arithmetic updates
		inline ColourValue& operator += ( const ColourValue& _rkVector )
		{
			r += _rkVector.r;
			g += _rkVector.g;
			b += _rkVector.b;
			a += _rkVector.a;

			invalidate();

			return *this;
		}

		inline ColourValue& operator -= ( const ColourValue& _rkVector )
		{
			r -= _rkVector.r;
			g -= _rkVector.g;
			b -= _rkVector.b;
			a -= _rkVector.a;

			invalidate();

			return *this;
		}

		inline ColourValue& operator *= ( const float _fScalar )
		{
			r *= _fScalar;
			g *= _fScalar;
			b *= _fScalar;
			a *= _fScalar;

			invalidate();

			return *this;
		}

		inline ColourValue& operator /= ( const float _fScalar )
		{
			assert( _fScalar != 0.0 );

			float fInv = 1.0f / _fScalar;

			r *= fInv;
			g *= fInv;
			b *= fInv;
			a *= fInv;

			invalidate();

			return *this;
		}

		//Set a colour value from Hue, Saturation and Brightness.
		//@param hue Hue value, scaled to the [0,1] range as opposed to the 0-360
		//@param saturation Saturation level, [0,1]
		//@param brightness Brightness level, [0,1]
		void setHSB( float _hue, float _saturation, float _brightness );

		inline float getA() const;
		inline float getR() const;
		inline float getG() const;
		inline float getB() const;

		void setA( const float _a );
		void setR( const float _r );
		void setG( const float _g );
		void setB( const float _b );

		inline bool isIdentity() const
		{
			return m_identity;
		}

		inline void invalidate() const
		{
			m_identity = false;
			m_invalidateARGB = true;
		}

	protected:
		float r,g,b,a;

		mutable bool m_identity;
		mutable ARGB m_argb;
		mutable bool m_invalidateARGB;
	};

	//////////////////////////////////////////////////////////////////////////
	inline float ColourValue::getA() const
	{
		return a;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float ColourValue::getR() const
	{
		return r;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float ColourValue::getG() const
	{
		return g;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float ColourValue::getB() const
	{
		return b;
	}
	//////////////////////////////////////////////////////////////////////////
	inline  float length_color( const ColourValue& _rColor )
	{
		return 1.0f;
	}
} // namespace
