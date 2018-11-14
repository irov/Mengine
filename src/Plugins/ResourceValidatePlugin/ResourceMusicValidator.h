#pragma once

#include "ResourceValidatorHelper.h"

#include "Engine/ResourceMusic.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceMusicValidator
        : public ResourceValidatorT<ResourceMusicPtr>
        , public Factorable
    {
    public:
        ResourceMusicValidator();
        ~ResourceMusicValidator() override;

    public:
        bool _validate( const ResourceMusicPtr & _resource ) override;
    };
}