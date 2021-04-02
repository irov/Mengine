#pragma once

#include "ResourceValidatorHelper.h"

#include "Engine/ResourceMusic.h"

namespace Mengine
{
    class ResourceMusicValidator
        : public ResourceValidatorT<ResourceMusicPtr>
    {
    public:
        ResourceMusicValidator();
        ~ResourceMusicValidator() override;

    public:
        bool _validate( const ResourceMusicPtr & _resource ) const override;
    };
}