#pragma once

#include "ResourceVideo.h"

#include "Kernel/Validator.h"

namespace Mengine
{
    class ResourceVideoValidator
        : public Validator<ResourceVideoPtr>
    {
    public:
        ResourceVideoValidator();
        ~ResourceVideoValidator() override;

    public:
        bool _validate( const ResourceVideoPtr & _resource ) const override;
    };
}
