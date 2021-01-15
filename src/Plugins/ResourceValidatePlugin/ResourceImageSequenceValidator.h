#pragma once

#include "ResourceValidatorHelper.h"

#include "Kernel/ResourceImageSequence.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceImageSequenceValidator
        : public ResourceValidatorT<ResourceImageSequencePtr>
        , public Factorable
    {
    public:
        ResourceImageSequenceValidator();
        ~ResourceImageSequenceValidator() override;

    public:
        bool _validate( const ResourceImageSequencePtr & _resource ) override;
    };
}