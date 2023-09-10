#pragma once

#include "Interface/ValidatorInterface.h"

namespace Mengine
{
    class BitmapFontGlyphValidator
        : public ValidatorInterface
    {
    public:
        bool validate( const FactorablePtr & _factorable ) const override;
    };
}