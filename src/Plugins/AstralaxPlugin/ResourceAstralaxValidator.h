#pragma once

#include "Plugins/ResourceValidatePlugin/ResourceValidatorHelper.h"

#include "ResourceAstralax.h"

namespace Mengine
{
    class ResourceAstralaxValidator
        : public ResourceValidatorT<ResourceAstralaxPtr>
    {
    public:
        ResourceAstralaxValidator();
        ~ResourceAstralaxValidator() override;

    public:
        bool _validate( const ResourceAstralaxPtr & _resource ) const override;
    };
}