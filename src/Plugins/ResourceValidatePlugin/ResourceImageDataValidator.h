#pragma once

#include "Kernel/ResourceImageData.h"
#include "Kernel/Validator.h"

namespace Mengine
{
    class ResourceImageDataValidator
        : public Validator<ResourceImageDataPtr>
    {
    public:
        ResourceImageDataValidator();
        ~ResourceImageDataValidator() override;

    public:
        bool _validate( const ResourceImageDataPtr & _resource ) const override;
    };
}