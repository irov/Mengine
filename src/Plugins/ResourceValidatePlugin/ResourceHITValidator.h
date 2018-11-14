#pragma once

#include "ResourceValidatorHelper.h"

#include "Engine/ResourceHIT.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceHITValidator
        : public ResourceValidatorT<ResourceHITPtr>
        , public Factorable
    {
    public:
        ResourceHITValidator();
        ~ResourceHITValidator() override;

    public:
        bool _validate( const ResourceHITPtr & _resource ) override;
    };
}