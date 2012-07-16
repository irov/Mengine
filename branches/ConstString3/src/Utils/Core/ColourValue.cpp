
#include "ColourValue.h"

namespace Menge
{

	const ColourValue ColourValue::ZERO = ColourValue(0.0,0.0,0.0,0.0);
	const ColourValue ColourValue::Black = ColourValue(0.0,0.0,0.0,1.f);
	const ColourValue ColourValue::White = ColourValue(1.0,1.0,1.0,1.f);
	const ColourValue ColourValue::Red = ColourValue(1.0,0.0,0.0,1.f);
	const ColourValue ColourValue::Green = ColourValue(0.0,1.0,0.0,1.f);
	const ColourValue ColourValue::Blue = ColourValue(0.0,0.0,1.0,1.f);

	//////////////////////////////////////////////////////////////////////////
	RGBA ColourValue::getAsRGBA() const
	{
		uint8 val8;
		uint32 val32 = 0;

		// Convert to 32bit pattern
		// (RGBA = 8888)

		// Red
		val8 = static_cast<uint8>(r * 255);
		val32 = val8 << 24;

		// Green
		val8 = static_cast<uint8>(g * 255);
		val32 += val8 << 16;

		// Blue
		val8 = static_cast<uint8>(b * 255);
		val32 += val8 << 8;

		// Alpha
		val8 = static_cast<uint8>(a * 255);
		val32 += val8;

		return val32;
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::updateARGB() const
	{
		m_invalidateARGB = false;

		uint8 val8;
		m_argb = 0;

		// Convert to 32bit pattern
		// (ARGB = 8888)

		// Alpha
		val8 = static_cast<uint8>(a * 255);
		m_argb = val8 << 24;

		// Red
		val8 = static_cast<uint8>(r * 255);
		m_argb += val8 << 16;

		// Green
		val8 = static_cast<uint8>(g * 255);
		m_argb += val8 << 8;

		// Blue
		val8 = static_cast<uint8>(b * 255);
		m_argb += val8;

		if( m_argb == 0xFFFFFFFF )
		{
			m_identity = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
	ARGB ColourValue::getAsARGB() const
#else
	BGRA ColourValue::getAsBGRA() const
#endif
	{
		uint8 val8;
		uint32 val32 = 0;

		// Convert to 32bit pattern
		// (ARGB = 8888)

		// Blue
		val8 = static_cast<uint8>(b * 255);
		val32 = val8 << 24;

		// Green
		val8 = static_cast<uint8>(g * 255);
		val32 += val8 << 16;

		// Red
		val8 = static_cast<uint8>(r * 255);
		val32 += val8 << 8;

		// Alpha
		val8 = static_cast<uint8>(a * 255);
		val32 += val8;


		return val32;
	}
	//////////////////////////////////////////////////////////////////////////
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
	RGBA ColourValue::getAsRGBA(void) const
#else
	ABGR ColourValue::getAsABGR(void) const
#endif
	{
		uint8 val8;
		uint32 val32 = 0;

		// Convert to 32bit pattern
		// (ABRG = 8888)

		// Alpha
		val8 = static_cast<uint8>(a * 255);
		val32 = val8 << 24;

		// Blue
		val8 = static_cast<uint8>(b * 255);
		val32 += val8 << 16;

		// Green
		val8 = static_cast<uint8>(g * 255);
		val32 += val8 << 8;

		// Red
		val8 = static_cast<uint8>(r * 255);
		val32 += val8;


		return val32;
	}
	//////////////////////////////////////////////////////////////////////////
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
	void ColourValue::setAsABGR( const ABGR _val )
#else
	void ColourValue::setAsRGBA( const RGBA _val )
#endif
	{
		uint32 val32 = _val;

		// Convert from 32bit pattern
		// (RGBA = 8888)

		// Red
		r = ((val32 >> 24) & 0xFF) / 255.0f;

		// Green
		g = ((val32 >> 16) & 0xFF) / 255.0f;

		// Blue
		b = ((val32 >> 8) & 0xFF) / 255.0f;

		// Alpha
		a = (val32 & 0xFF) / 255.0f;
		
		invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
	void ColourValue::setAsBGRA( const BGRA _val )
#else
	void ColourValue::setAsARGB( const ARGB _val )
#endif
	{
		m_argb = _val;

		if( m_argb == 0xFFFFFFFF )
		{
			m_invalidateARGB = false;
			m_identity = true;

			r = g = b = a = 1.f;
		}
		else
		{
			m_invalidateARGB = false;
			m_identity = false;
			// Convert from 32bit pattern
			// (ARGB = 8888)

			// Alpha
			a = ((m_argb >> 24) & 0xFF) / 255.0f;

			// Red
			r = ((m_argb >> 16) & 0xFF) / 255.0f;

			// Green
			g = ((m_argb >> 8) & 0xFF) / 255.0f;

			// Blue
			b = (m_argb & 0xFF) / 255.0f;			
		}
	}
	//////////////////////////////////////////////////////////////////////////
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
	void ColourValue::setAsARGB( const ARGB _val )
#else
	void ColourValue::setAsBGRA( const BGRA _val )
#endif
	{
		uint32 val32 = _val;

		// Convert from 32bit pattern
		// (ARGB = 8888)

		// Blue
		b = ((val32 >> 24) & 0xFF) / 255.0f;

		// Green
		g = ((val32 >> 16) & 0xFF) / 255.0f;

		// Red
		r = ((val32 >> 8) & 0xFF) / 255.0f;

		// Alpha
		a = (val32 & 0xFF) / 255.0f;

		invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
	void ColourValue::setAsRGBA( const RGBA _val )
#else
	void ColourValue::setAsABGR( const ABGR _val )
#endif
	{
		uint32 val32 = _val;

		// Convert from 32bit pattern
		// (ABGR = 8888)

		// Alpha
		a = ((val32 >> 24) & 0xFF) / 255.0f;

		// Blue
		b = ((val32 >> 16) & 0xFF) / 255.0f;

		// Green
		g = ((val32 >> 8) & 0xFF) / 255.0f;

		// Red
		r = (val32 & 0xFF) / 255.0f;

		invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ColourValue::operator==( const ColourValue& _rhs ) const
	{
		return (r == _rhs.r &&
			g == _rhs.g &&
			b == _rhs.b &&
			a == _rhs.a);
	}
	//////////////////////////////////////////////////////////////////////////
	bool ColourValue::operator!=( const ColourValue& _rhs ) const
	{
		return !(*this == _rhs);
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setHSB( float _hue, float _saturation, float _brightness )
	{
		// wrap hue
		if ( _hue > 1.0f)
		{ 
			_hue -= (int)_hue;
		}
		else if (_hue < 0.0f)
		{
			_hue += (int)_hue + 1;
		}
		// clamp saturation / brightness
		_saturation = (std::min)(_saturation, (float)1.0);
		_saturation = (std::max)(_saturation, (float)0.0);
		_brightness = (std::min)(_brightness, (float)1.0);
		_brightness = (std::max)(_brightness, (float)0.0);

		if (_brightness == 0.0f)
		{   
			// early exit, this has to be black
			r = g = b = 0.0f;
			return;
		}

		if (_saturation == 0.0f)
		{   
			// early exit, this has to be grey

			r = g = b = _brightness;
			return;
		}


		float hueDomain  = _hue * 6.0f;
		if (hueDomain >= 6.0f)
		{
			// wrap around, and allow mathematical errors
			hueDomain = 0.0f;
		}
		unsigned short domain = (unsigned short)hueDomain;
		float f1 = _brightness * (1 - _saturation);
		float f2 = _brightness * (1 - _saturation * (hueDomain - domain));
		float f3 = _brightness * (1 - _saturation * (1 - (hueDomain - domain)));

		switch (domain)
		{
		case 0:
			// red domain; green ascends
			r = _brightness;
			g = f3;
			b = f1;
			break;
		case 1:
			// yellow domain; red descends
			r = f2;
			g = _brightness;
			b = f1;
			break;
		case 2:
			// green domain; blue ascends
			r = f1;
			g = _brightness;
			b = f3;
			break;
		case 3:
			// cyan domain; green descends
			r = f1;
			g = f2;
			b = _brightness;
			break;
		case 4:
			// blue domain; red ascends
			r = f3;
			g = f1;
			b = _brightness;
			break;
		case 5:
			// magenta domain; blue descends
			r = _brightness;
			g = f1;
			b = f2;
			break;
		}

		invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setA( const float _a )
	{
		a = _a;
		invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setR( const float _r )
	{
		r = _r;
		invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setG( const float _g )
	{
		g = _g;
		invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setB( const float _b )
	{
		b = _b;
		invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
}

