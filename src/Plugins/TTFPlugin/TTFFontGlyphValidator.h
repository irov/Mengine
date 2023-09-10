#pragma once

#include "TTFFontGlyph.h"

#include "Kernel/Validator.h"

namespace Mengine
{
    class TTFFontGlyphValidator
        : public Validator<TTFFontGlyphPtr>
    {
    public:
        bool _validate( const TTFFontGlyphPtr & _factorable ) const override;
    };
}