#pragma once

#include "Plugins/ResourceValidatePlugin/ResourceValidatorHelper.h"

#include "ResourceMovie.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ResourceMovieValidator
        : public ResourceValidatorT<ResourceMoviePtr>
        , public Factorable
    {
    public:
        ResourceMovieValidator();
        ~ResourceMovieValidator() override;

    public:
        bool _validate( const ResourceMoviePtr & _resource ) override;
    };
}