#pragma once

#include "ResourceValidatorHelper.h"

#include "Engine/ResourceFile.h"

namespace Mengine
{
    class ResourceFileValidator
        : public ResourceValidatorT<ResourceFilePtr>
    {
    public:
        ResourceFileValidator();
        ~ResourceFileValidator() override;

    public:
        bool _validate( const ResourceFilePtr & _resource ) const override;
    };
}