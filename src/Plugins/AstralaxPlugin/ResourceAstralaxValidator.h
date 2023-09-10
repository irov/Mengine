#pragma once

#include "ResourceAstralax.h"

#include "Kernel/Validator.h"

namespace Mengine
{
    class ResourceAstralaxValidator
        : public Validator<ResourceAstralaxPtr>
    {
    public:
        ResourceAstralaxValidator();
        ~ResourceAstralaxValidator() override;

    public:
        bool _validate( const ResourceAstralaxPtr & _resource ) const override;
    };
}