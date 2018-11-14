#pragma once

#include "Plugins/ResourceValidatePlugin/ResourceValidatorHelper.h"

#include "ResourceAstralax.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceAstralaxValidator
        : public ResourceValidatorT<ResourceAstralaxPtr>
        , public Factorable
    {
    public:
        ResourceAstralaxValidator();
        ~ResourceAstralaxValidator() override;

    public:
        bool _validate( const ResourceAstralaxPtr & _resource ) override;
    };
}