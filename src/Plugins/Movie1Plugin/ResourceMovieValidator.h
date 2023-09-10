#pragma once

#include "ResourceMovie.h"

#include "Kernel/Validator.h"

namespace Mengine
{
    class ResourceMovieValidator
        : public Validator<ResourceMoviePtr>
    {
    public:
        ResourceMovieValidator();
        ~ResourceMovieValidator() override;

    public:
        bool _validate( const ResourceMoviePtr & _resource ) const override;
    };
}