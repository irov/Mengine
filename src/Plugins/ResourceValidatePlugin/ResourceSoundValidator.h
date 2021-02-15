#pragma once

#include "ResourceValidatorHelper.h"

#include "Kernel/ResourceSound.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceSoundValidator
        : public ResourceValidatorT<ResourceSoundPtr>
        , public Factorable
    {
    public:
        ResourceSoundValidator();
        ~ResourceSoundValidator() override;

    public:
        bool _validate( const ResourceSoundPtr & _resource ) override;
    };
}