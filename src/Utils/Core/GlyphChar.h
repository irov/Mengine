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
        inline void setCode( uint32_t _code );       
        inline uint32_t getCode() const;

    protected:
        uint32_t m_utf8;
    };
	//////////////////////////////////////////////////////////////////////////
	inline void GlyphCode::setCode( uint32_t _code )
	{
		m_utf8 = _code;
	}
	//////////////////////////////////////////////////////////////////////////
	inline uint32_t GlyphCode::getCode() const
	{
		return m_utf8;
	}
	//////////////////////////////////////////////////////////////////////////
    struct GlyphCharLess
    {
        inline bool operator()( const GlyphCode & _left, const GlyphCode & _right ) const
        {
            uint32_t lid = _left.getCode();
            uint32_t rid = _right.getCode();

            return lid < rid;
        }
    };
}