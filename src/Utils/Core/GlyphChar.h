#   pragma once

namespace Menge
{
    class GlyphCode
    {
    public:
        GlyphCode()
            : m_utf8((uint32_t)-1)
        {
        }

    public:
        inline bool empty() const;

    public:
        inline void setUTF8( uint32_t _code );       
        inline uint32_t getUTF8() const;

    protected:
        uint32_t m_utf8;
    };
    //////////////////////////////////////////////////////////////////////////
    inline bool GlyphCode::empty() const
    {
        return m_utf8 == (uint32_t)-1;
    }
	//////////////////////////////////////////////////////////////////////////
	inline void GlyphCode::setUTF8( uint32_t _code )
	{
		m_utf8 = _code;
	}
	//////////////////////////////////////////////////////////////////////////
	inline uint32_t GlyphCode::getUTF8() const
	{
		return m_utf8;
	}
	//////////////////////////////////////////////////////////////////////////
    inline bool operator == ( const GlyphCode & _l, const GlyphCode & _r )
    {
        return _l.getUTF8() == _r.getUTF8();
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool operator != ( const GlyphCode & _l, const GlyphCode & _r )
    {
        return _l.getUTF8() != _r.getUTF8();
    }
}