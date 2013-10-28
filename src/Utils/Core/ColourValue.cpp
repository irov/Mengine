#   include "ColourValue.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::updateARGB() const
	{
		m_invalidateARGB = false;

		uint8 a8 = static_cast<uint8>(m_a * 255.f);
		uint8 r8 = static_cast<uint8>(m_r * 255.f);
		uint8 g8 = static_cast<uint8>(m_g * 255.f);
		uint8 b8 = static_cast<uint8>(m_b * 255.f);

		m_argb = (a8 << 24) + (r8 << 16) + (g8 << 8) + b8;

		if( m_argb == 0xFFFFFFFF )
		{
			m_identity = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setARGB( float _a, float _r, float _g, float _b )
	{
		m_a = _a;
		m_r = _r;
		m_g = _g;
		m_b = _b;

		this->invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::setAsARGB( const ARGB _val )
	{
		m_argb = _val;

		if( _val == 0xFFFFFFFF )
		{
			m_invalidateARGB = false;
			m_identity = true;

			m_r = 1.f;
			m_g = 1.f;
			m_b = 1.f;
			m_a = 1.f;

			return;
		}

		m_a = ((m_argb >> 24) & 0xFF) / 255.f;
		m_r = ((m_argb >> 16) & 0xFF) / 255.f;
		m_g = ((m_argb >> 8) & 0xFF) / 255.f;
		m_b = (m_argb & 0xFF) / 255.f;

		m_invalidateARGB = false;
		m_identity = false;
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

