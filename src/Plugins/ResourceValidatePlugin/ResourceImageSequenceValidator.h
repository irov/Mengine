#pragma once

#include "Kernel/ResourceImageSequence.h"
#include "Kernel/Validator.h"

namespace Mengine
{
    class ResourceImageSequenceValidator
        : public Validator<ResourceImageSequencePtr>
    {
    public:
        ResourceImageSequenceValidator();
        ~ResourceImageSequenceValidator() override;

    public:
        bool _validate( const ResourceImageSequencePtr & _resource ) const override;
    };
}