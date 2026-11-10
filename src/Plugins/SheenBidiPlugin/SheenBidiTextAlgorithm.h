#pragma once

#include "Interface/TextAlgorithmInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class SheenBidiTextAlgorithm
        : public TextAlgorithmInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SheenBidiTextAlgorithm );

    public:
        SheenBidiTextAlgorithm();
        ~SheenBidiTextAlgorithm() override;

    public:
        TextBidirectionalInterfacePtr createBidirectional( const U32String & _text, const DocumentInterfacePtr & _doc ) const override;
        bool getScriptSegments( const uint32_t * _text, size_t _size, VectorTextScriptSegments * const _segments ) const override;
        Char32 getMirroredCodepoint( Char32 _code ) const override;
        bool isCombiningMark( Char32 _code ) const override;
    };
}
