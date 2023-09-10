#pragma once

#include "Kernel/ResourceSound.h"
#include "Kernel/Validator.h"

namespace Mengine
{
    class ResourceSoundValidator
        : public Validator<ResourceSoundPtr>
    {
    public:
        ResourceSoundValidator();
        ~ResourceSoundValidator() override;

    public:
        bool _validate( const ResourceSoundPtr & _resource ) const override;
    };
}