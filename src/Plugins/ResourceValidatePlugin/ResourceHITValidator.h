#pragma once

#include "ResourceValidatorHelper.h"

#include "Engine/ResourceHIT.h"

namespace Mengine
{
    class ResourceHITValidator
        : public ResourceValidatorT<ResourceHITPtr>
    {
    public:
        ResourceHITValidator();
        ~ResourceHITValidator() override;

    public:
        bool _validate( const ResourceHITPtr & _resource ) const override;
    };
}