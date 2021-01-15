#pragma once

#include "ResourceValidatorHelper.h"

#include "Kernel/ResourceImageData.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceImageDataValidator
        : public ResourceValidatorT<ResourceImageDataPtr>
        , public Factorable
    {
    public:
        ResourceImageDataValidator();
        ~ResourceImageDataValidator() override;

    public:
        bool _validate( const ResourceImageDataPtr & _resource ) override;
    };
}