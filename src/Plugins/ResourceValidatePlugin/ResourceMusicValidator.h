#pragma once

#include "Engine/ResourceMusic.h"
#include "Kernel/Validator.h"

namespace Mengine
{
    class ResourceMusicValidator
        : public Validator<ResourceMusicPtr>
    {
    public:
        ResourceMusicValidator();
        ~ResourceMusicValidator() override;

    public:
        bool _validate( const ResourceMusicPtr & _resource ) const override;
    };
}