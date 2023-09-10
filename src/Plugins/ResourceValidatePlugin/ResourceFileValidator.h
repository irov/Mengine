#pragma once

#include "Engine/ResourceFile.h"

#include "Kernel/Validator.h"

namespace Mengine
{
    class ResourceFileValidator
        : public Validator<ResourceFilePtr>
    {
    public:
        ResourceFileValidator();
        ~ResourceFileValidator() override;

    public:
        bool _validate( const ResourceFilePtr & _resource ) const override;
    };
}