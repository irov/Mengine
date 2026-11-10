#pragma once

#include "Interface/TextBidirectionalInterface.h"

#include "Kernel/Factorable.h"

#include "SheenBidi/SBAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SheenBidiTextBidirectional
        : public TextBidirectionalInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SheenBidiTextBidirectional );

    public:
        SheenBidiTextBidirectional();
        ~SheenBidiTextBidirectional() override;

    public:
        bool initialize( const U32String & _text );

    public:
        bool getVisualSegments( size_t _begin, size_t _end, VectorTextBidirectionalSegments * const _segments ) const override;

    private:
        SheenBidiTextBidirectional( const SheenBidiTextBidirectional & _other );
        SheenBidiTextBidirectional & operator = ( const SheenBidiTextBidirectional & _other );

    private:
        typedef Vector<uint32_t> VectorCodepoints;
        typedef Vector<SBParagraphRef> VectorParagraphs;

    private:
        VectorCodepoints m_codepoints;
        SBAlgorithmRef m_algorithm;
        VectorParagraphs m_paragraphs;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SheenBidiTextBidirectional> SheenBidiTextBidirectionalPtr;
    //////////////////////////////////////////////////////////////////////////
}
