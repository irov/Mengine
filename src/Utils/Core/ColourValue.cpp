#   include "ColourValue.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::updateARGB() const
	{
		m_invalidateARGB = false;

#   ifdef MENGE_RENDER_TEXTURE_RGBA
		uint8_t a8 = static_cast<uint8_t>(m_a * 255.f);
		uint8_t b8 = static_cast<uint8_t>(m_r * 255.f);
		uint8_t g8 = static_cast<uint8_t>(m_g * 255.f);
		uint8_t r8 = static_cast<uint8_t>(m_b * 255.f);
#	else // MENGE_RENDER_GL
		uint8_t a8 = static_cast<uint8_t>(m_a * 255.f);
		uint8_t r8 = static_cast<uint8_t>(m_r * 255.f);
		uint8_t g8 = static_cast<uint8_t>(m_g * 255.f);
		uint8_t b8 = static_cast<uint8_t>(m_b * 255.f);
#	endif // MENGE_RENDER_GL

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

		const float rgba_255 = 1.f / 255.f;

#   ifdef MENGE_RENDER_TEXTURE_RGBA
		m_a = ((m_argb >> 24) & 0xFF) * rgba_255;
		m_b = ((m_argb >> 16) & 0xFF) * rgba_255;
		m_g = ((m_argb >> 8) & 0xFF) * rgba_255;
		m_r = (m_argb & 0xFF) * rgba_255;
#	else // MENGE_RENDER_GL
		m_a = ((m_argb >> 24) & 0xFF) * rgba_255;
		m_r = ((m_argb >> 16) & 0xFF) * rgba_255;
		m_g = ((m_argb >> 8) & 0xFF) * rgba_255;
		m_b = (m_argb & 0xFF) * rgba_255;
#	endif // MENGE_RENDER_GL

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

