#pragma once

#include "ResourceValidatorHelper.h"

#include "Kernel/ResourceImageDefault.h"

namespace Mengine
{
    class ResourceImageDefaultValidator
        : public ResourceValidatorT<ResourceImageDefaultPtr>
    {
    public:
        ResourceImageDefaultValidator();
        ~ResourceImageDefaultValidator() override;

    public:
        bool _validate( const ResourceImageDefaultPtr & _resource ) const override;
    };
}