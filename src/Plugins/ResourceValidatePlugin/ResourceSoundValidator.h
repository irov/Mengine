#pragma once

#include "ResourceValidatorHelper.h"

#include "Kernel/ResourceSound.h"

namespace Mengine
{
    class ResourceSoundValidator
        : public ResourceValidatorT<ResourceSoundPtr>
    {
    public:
        ResourceSoundValidator();
        ~ResourceSoundValidator() override;

    public:
        bool _validate( const ResourceSoundPtr & _resource ) const override;
    };
}