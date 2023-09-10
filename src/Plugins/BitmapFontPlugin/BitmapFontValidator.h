#pragma once

#include "Interface/ValidatorInterface.h"

namespace Mengine
{
    class BitmapFontValidator
        : public ValidatorInterface
    {
    public:
        bool validate( const FactorablePtr & _factorable ) const override;
    };
}