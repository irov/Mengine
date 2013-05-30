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
        void setCode( size_t _code )
        {
            m_utf8 = _code;
        }
        
    public:
        size_t getCode() const
        {
            return m_utf8;
        }

    protected:
        size_t m_utf8;
    };

    struct GlyphCharLess
    {
        bool operator()( const GlyphChar & _left, const GlyphChar & _right ) const
        {
            size_t lid = _left.getCode();
            size_t rid = _right.getCode();

            return lid < rid;
        }
    };
}