#pragma once

#include "ResourceValidatorHelper.h"

#include "Kernel/ResourceImageDefault.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceImageDefaultValidator
        : public ResourceValidatorT<ResourceImageDefaultPtr>
        , public Factorable
    {
    public:
        ResourceImageDefaultValidator();
        ~ResourceImageDefaultValidator() override;

    public:
        bool _validate( const ResourceImageDefaultPtr & _resource ) override;
    };
}