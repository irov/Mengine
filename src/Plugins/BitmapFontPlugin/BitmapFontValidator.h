#pragma once

#include "Interface/FontValidatorInterface.h"

namespace Mengine
{
    class BitmapFontValidator
        : public FontValidatorInterface
    {
    public:
        bool validate( const TextFontInterfacePtr & _textFont ) const override;
    };
}