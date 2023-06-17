#pragma once

#include "Interface/FontValidatorInterface.h"

namespace Mengine
{
    class TTFFontValidator
        : public FontValidatorInterface
    {
    public:
        bool validate( const FontInterfacePtr & _font ) const override;
    };
}