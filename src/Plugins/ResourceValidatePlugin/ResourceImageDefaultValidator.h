#pragma once

#include "Kernel/ResourceImageDefault.h"
#include "Kernel/Validator.h"

namespace Mengine
{
    class ResourceImageDefaultValidator
        : public Validator<ResourceImageDefaultPtr>
    {
    public:
        ResourceImageDefaultValidator();
        ~ResourceImageDefaultValidator() override;

    public:
        bool _validate( const ResourceImageDefaultPtr & _resource ) const override;
    };
}