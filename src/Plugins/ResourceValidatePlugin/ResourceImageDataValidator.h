#pragma once

#include "ResourceValidatorHelper.h"

#include "Kernel/ResourceImageData.h"

namespace Mengine
{
    class ResourceImageDataValidator
        : public ResourceValidatorT<ResourceImageDataPtr>
    {
    public:
        ResourceImageDataValidator();
        ~ResourceImageDataValidator() override;

    public:
        bool _validate( const ResourceImageDataPtr & _resource ) const override;
    };
}