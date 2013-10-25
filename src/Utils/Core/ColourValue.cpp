#   include "ColourValue.h"

#   include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::updateARGB() const
	{
		m_invalidateARGB = false;

		uint8 a8 = static_cast<uint8>(m_a * 255);
		uint8 r8 = static_cast<uint8>(m_r * 255);
		uint8 g8 = static_cast<uint8>(m_g * 255);
		uint8 b8 = static_cast<uint8>(m_b * 255);

		m_argb = (a8 << 24) + (r8 << 16) + (g8 << 8) + b8;

		if( m_argb == 0xFFFFFFFF )
		{
			m_identity = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	RGBA ColourValue::getAsRGBA() const
	{
		uint8 a8 = static_cast<uint8>(m_a * 255);
		uint8 r8 = static_cast<uint8>(m_r * 255);
		uint8 g8 = static_cast<uint8>(m_g * 255);
		uint8 b8 = static_cast<uint8>(m_b * 255);

		uint32 val32 = (r8 << 24) + (g8 << 16) + (b8 << 8) + a8;

		return val32;
	}
	//////////////////////////////////////////////////////////////////////////
	BGRA ColourValue::getAsBGRA() const
	{
		uint8 a8 = static_cast<uint8>(m_a * 255);
		uint8 r8 = static_cast<uint8>(m_r * 255);
		uint8 g8 = static_cast<uint8>(m_g * 255);
		uint8 b8 = static_cast<uint8>(m_b * 255);

		uint32 val32 = (b8 << 24) + (g8 << 16) + (r8 << 8) + a8;

		return val32;
	}
	//////////////////////////////////////////////////////////////////////////
	ABGR ColourValue::getAsABGR(void) const
	{
		uint8 a8 = static_cast<uint8>(m_a * 255);
		uint8 r8 = static_cast<uint8>(m_r * 255);
		uint8 g8 = static_cast<uint8>(m_g * 255);
		uint8 b8 = static_cast<uint8>(m_b * 255);

		uint32 val32 = (a8 << 24) + (b8 << 16) + (g8 << 8) + r8;

		return val32;
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setRGBA( float _r, float _g, float _b, float _a )
	{
		m_r = _r;
		m_g = _g;
		m_b = _b;
		m_a = _a;

		this->invalidate();
	}
	//////////////////////////////////////////////////////////////////////////	
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
	void ColourValue::setAsABGR( const ABGR _val )
#else
	void ColourValue::setAsRGBA( const RGBA _val )
#endif
	{
		m_argb = _val;

		if( m_argb == 0xFFFFFFFF )
		{
			m_invalidateARGB = false;
			m_identity = true;

			m_r = 1.f;
			m_g = 1.f;
			m_b = 1.f;
			m_a = 1.f;
		}
		else
		{
			m_invalidateARGB = false;
			m_identity = false;

			m_r = ((m_argb >> 24) & 0xFF) / 255.0f;
			m_g = ((m_argb >> 16) & 0xFF) / 255.0f;
			m_b = ((m_argb >> 8) & 0xFF) / 255.0f;
			m_a = (m_argb & 0xFF) / 255.0f;
		}
		
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

			m_r = 1.f;
			m_g = 1.f;
			m_b = 1.f;
			m_a = 1.f;
		}
		else
		{
			m_invalidateARGB = false;
			m_identity = false;

			m_a = ((m_argb >> 24) & 0xFF) / 255.f;
			m_r = ((m_argb >> 16) & 0xFF) / 255.f;
			m_g = ((m_argb >> 8) & 0xFF) / 255.f;
			m_b = (m_argb & 0xFF) / 255.f;
		}
	}
	//////////////////////////////////////////////////////////////////////////
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
	void ColourValue::setAsARGB( const ARGB _val )
#else
	void ColourValue::setAsBGRA( const BGRA _val )
#endif
	{
		m_argb = _val;

		if( m_argb == 0xFFFFFFFF )
		{
			m_invalidateARGB = false;
			m_identity = true;

			m_r = 1.f;
			m_g = 1.f;
			m_b = 1.f;
			m_a = 1.f;
		}
		else
		{
			m_invalidateARGB = false;
			m_identity = false;

			m_b = ((m_argb >> 24) & 0xFF) / 255.f;
			m_g = ((m_argb >> 16) & 0xFF) / 255.f;
			m_r = ((m_argb >> 8) & 0xFF) / 255.f;
			m_a = (m_argb & 0xFF) / 255.f;
		}

		this->invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
	void ColourValue::setAsRGBA( const RGBA _val )
#else
	void ColourValue::setAsABGR( const ABGR _val )
#endif
	{
		m_argb = _val;

		if( m_argb == 0xFFFFFFFF )
		{
			m_invalidateARGB = false;
			m_identity = true;

			m_r = 1.f;
			m_g = 1.f;
			m_b = 1.f;
			m_a = 1.f;
		}
		else
		{
			m_invalidateARGB = false;
			m_identity = false;

			m_a = ((m_argb >> 24) & 0xFF) / 255.f;
			m_b = ((m_argb >> 16) & 0xFF) / 255.f;
			m_g = ((m_argb >> 8) & 0xFF) / 255.f;
			m_r = (m_argb & 0xFF) / 255.f;
		}

		this->invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setA( const float _a )
	{
		m_a = _a;
		
        this->invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setR( const float _r )
	{
		m_r = _r;

		this->invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setG( const float _g )
	{
		m_g = _g;

		this->invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setB( const float _b )
	{
		m_b = _b;

		this->invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
}

