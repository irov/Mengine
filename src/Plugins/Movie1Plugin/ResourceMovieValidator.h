#pragma once

#include "Plugins/ResourceValidatePlugin/ResourceValidatorHelper.h"

#include "ResourceMovie.h"

namespace Mengine
{
    class ResourceMovieValidator
        : public ResourceValidatorT<ResourceMoviePtr>
    {
    public:
        ResourceMovieValidator();
        ~ResourceMovieValidator() override;

    public:
        bool _validate( const ResourceMoviePtr & _resource ) const override;
    };
}