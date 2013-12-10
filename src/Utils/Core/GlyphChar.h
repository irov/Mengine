#   pragma once

namespace Menge
{
    class GlyphChar
    {
    public:
        GlyphChar()
            : m_utf8((size_t)-1)
        {
        }

    public:
        inline void setCode( size_t _code );       
        inline size_t getCode() const;

    protected:
        size_t m_utf8;
    };
	//////////////////////////////////////////////////////////////////////////
	inline void GlyphChar::setCode( size_t _code )
	{
		m_utf8 = _code;
	}
	//////////////////////////////////////////////////////////////////////////
	inline size_t GlyphChar::getCode() const
	{
		return m_utf8;
	}
	//////////////////////////////////////////////////////////////////////////
    struct GlyphCharLess
    {
        inline bool operator()( const GlyphChar & _left, const GlyphChar & _right ) const
        {
            size_t lid = _left.getCode();
            size_t rid = _right.getCode();

            return lid < rid;
        }
    };
}