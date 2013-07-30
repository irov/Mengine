#   include "ColourValue.h"

#   include <algorithm>

namespace Menge
{
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
		m_argb += val8 << 24;

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
	BGRA ColourValue::getAsBGRA() const
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
	ABGR ColourValue::getAsABGR(void) const
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
		
		this->invalidate();
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

		this->invalidate();
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

		this->invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ColourValue::operator == ( const ColourValue& _rhs ) const
	{
		return (r == _rhs.r &&
			g == _rhs.g &&
			b == _rhs.b &&
			a == _rhs.a);
	}
	//////////////////////////////////////////////////////////////////////////
	bool ColourValue::operator != ( const ColourValue& _rhs ) const
	{
		return !(*this == _rhs);
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setA( const float _a )
	{
		a = _a;
		
        this->invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setR( const float _r )
	{
		r = _r;

		this->invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setG( const float _g )
	{
		g = _g;

		this->invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setB( const float _b )
	{
		b = _b;

		this->invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
}

