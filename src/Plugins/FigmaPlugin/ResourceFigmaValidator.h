#pragma once

#include "ResourceFigma.h"

#include "Kernel/Validator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceFigmaValidator
        : public Validator<ResourceFigmaPtr>
    {
    public:
        ResourceFigmaValidator();
        ~ResourceFigmaValidator() override;

    protected:
        bool _validate( const ResourceFigmaPtr & _resource ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceFigmaValidator> ResourceFigmaValidatorPtr;
    //////////////////////////////////////////////////////////////////////////
}
