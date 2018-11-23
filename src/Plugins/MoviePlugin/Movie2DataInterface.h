#pragma once

#include "Interface/DataInterface.h"

#include "movie/movie.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie2DataInterface
        : public DataInterface
    {
    public:
        virtual const aeMovieData * getMovieData() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2DataInterface> Movie2DataInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}