#pragma once

#include "ResourceValidatorHelper.h"

#include "Kernel/ResourceImageSequence.h"

namespace Mengine
{
    class ResourceImageSequenceValidator
        : public ResourceValidatorT<ResourceImageSequencePtr>
    {
    public:
        ResourceImageSequenceValidator();
        ~ResourceImageSequenceValidator() override;

    public:
        bool _validate( const ResourceImageSequencePtr & _resource ) const override;
    };
}