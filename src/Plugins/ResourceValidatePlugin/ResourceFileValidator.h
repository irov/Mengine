#pragma once

#include "ResourceValidatorHelper.h"

#include "Engine/ResourceFile.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceFileValidator
        : public ResourceValidatorT<ResourceFilePtr>
        , public Factorable
    {
    public:
        ResourceFileValidator();
        ~ResourceFileValidator() override;

    public:
        bool _validate( const ResourceFilePtr & _resource ) override;
    };
}