#pragma once

#include "Plugins/ResourceValidatePlugin/ResourceValidatorHelper.h"

#include "ResourceVideo.h"

namespace Mengine
{
    class ResourceVideoValidator
        : public ResourceValidatorT<ResourceVideoPtr>
    {
    public:
        ResourceVideoValidator();
        ~ResourceVideoValidator() override;

    public:
        bool _validate( const ResourceVideoPtr & _resource ) const override;
    };
}
