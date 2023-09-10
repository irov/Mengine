#pragma once

#include "TTFFont.h"

#include "Kernel/Validator.h"

namespace Mengine
{
    class TTFFontValidator
        : public Validator<TTFFontPtr>
    {
    public:
        bool _validate( const TTFFontPtr & _factorable ) const override;
    };
}