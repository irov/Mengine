#pragma once

#include "Engine/ResourceHIT.h"

#include "Kernel/Validator.h"

namespace Mengine
{
    class ResourceHITValidator
        : public Validator<ResourceHITPtr>
    {
    public:
        ResourceHITValidator();
        ~ResourceHITValidator() override;

    public:
        bool _validate( const ResourceHITPtr & _resource ) const override;
    };
}