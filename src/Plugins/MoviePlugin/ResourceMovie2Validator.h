#pragma once

#include "Plugins/ResourceValidatePlugin/ResourceValidatorHelper.h"

#include "ResourceMovie2.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceMovie2Validator
        : public ResourceValidatorT<ResourceMovie2Ptr>
        , public Factorable
    {
    public:
        ResourceMovie2Validator();
        ~ResourceMovie2Validator() override;

    public:
        bool _validate( const ResourceMovie2Ptr & _resource ) override;
    };
}