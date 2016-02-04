#   include "ColourValue.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		ColourValue_ARGB makeARGB( float _r, float _g, float _b, float _a )
		{
			const float rgba_255 = 255.f;

#   ifdef MENGE_RENDER_TEXTURE_RGBA
			uint8_t a8 = static_cast<uint8_t>(_a * rgba_255);
			uint8_t b8 = static_cast<uint8_t>(_r * rgba_255);
			uint8_t g8 = static_cast<uint8_t>(_g * rgba_255);
			uint8_t r8 = static_cast<uint8_t>(_b * rgba_255);
#	else // MENGE_RENDER_TEXTURE_RGBA
			uint8_t a8 = static_cast<uint8_t>(_a * rgba_255);
			uint8_t r8 = static_cast<uint8_t>(_r * rgba_255);
			uint8_t g8 = static_cast<uint8_t>(_g * rgba_255);
			uint8_t b8 = static_cast<uint8_t>(_b * rgba_255);
#	endif // MENGE_RENDER_TEXTURE_RGBA

			ColourValue_ARGB argb = (a8 << 24) | (r8 << 16) | (g8 << 8) | (b8 << 0);

			return argb;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ColourValue::updateARGB_() const
	{		
		m_argb = Helper::makeARGB( m_r, m_g, m_b, m_a );

		if( m_argb == 0xFFFFFFFF )
		{
			m_invalidateARGB = COLOUR_VALUE_INVALIDATE_IDENTITY;
		}
		else
		{
			m_invalidateARGB = COLOUR_VALUE_INVALIDATE_FALSE;
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
	void ColourValue::setAsARGB( const ColourValue_ARGB _val )
	{
		m_argb = _val;

		if( _val == 0xFFFFFFFF )
		{			
			m_r = 1.f;
			m_g = 1.f;
			m_b = 1.f;
			m_a = 1.f;

			m_invalidateARGB = COLOUR_VALUE_INVALIDATE_IDENTITY;

			return;
		}

#   ifdef MENGE_RENDER_TEXTURE_RGBA
		uint8_t a8 = (m_argb >> 24) & 0xFF;
		uint8_t b8 = (m_argb >> 16) & 0xFF;
		uint8_t g8 = (m_argb >> 8) & 0xFF;
		uint8_t r8 = (m_argb >> 0) & 0xFF;
#	else // MENGE_RENDER_TEXTURE_RGBA
		uint8_t a8 = (m_argb >> 24) & 0xFF;
		uint8_t r8 = (m_argb >> 16) & 0xFF;
		uint8_t g8 = (m_argb >> 8) & 0xFF;
		uint8_t b8 = (m_argb >> 0) & 0xFF;
#	endif // MENGE_RENDER_TEXTURE_RGBA

		const float rgba_1_div_255 = 1.f / 255.f;

		m_a = (float)(a8) * rgba_1_div_255;
		m_b = (float)(b8) * rgba_1_div_255;
		m_g = (float)(g8) * rgba_1_div_255;
		m_r = (float)(r8) * rgba_1_div_255;		

		m_invalidateARGB = COLOUR_VALUE_INVALIDATE_FALSE;
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

