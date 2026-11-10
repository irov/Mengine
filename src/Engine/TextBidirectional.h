#pragma once

#include "Interface/TextBidirectionalInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TextBidirectional
    {
    public:
        TextBidirectional();
        ~TextBidirectional();

    public:
        bool initialize( const U32String & _text );
        bool getVisualSegments( size_t _begin, size_t _end, VectorTextBidirectionalSegments * const _segments ) const;

    private:
        TextBidirectional( const TextBidirectional & _other );
        TextBidirectional & operator = ( const TextBidirectional & _other );

    private:
        TextBidirectionalInterfacePtr m_bidirectional;
        size_t m_textSize;
    };
    //////////////////////////////////////////////////////////////////////////
}
