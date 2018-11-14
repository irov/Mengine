#pragma once

#include "Plugins/ResourceValidatePlugin/ResourceValidatorHelper.h"

#include "ResourceVideo.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceVideoValidator
        : public ResourceValidatorT<ResourceVideoPtr>
        , public Factorable
    {
    public:
        ResourceVideoValidator();
        ~ResourceVideoValidator() override;

    public:
        bool _validate( const ResourceVideoPtr & _resource ) override;
    };
}
