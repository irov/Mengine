#pragma once

#include "Interface/DataInterface.h"

#include "Kernel/Resource.h"

#include "movie/movie.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie2DataInterface
        : public DataInterface
    {
    public:
        virtual const aeMovieData * getMovieData() const = 0;

    public:
        virtual bool acquireCompositionData( const aeMovieCompositionData * _composition ) = 0;
        virtual void releaseCompositionData( const aeMovieCompositionData * _composition ) = 0;

    public:
        typedef Lambda<void( const ResourcePtr & )> LambdaResource;
        virtual void visitCompositionDataResources( const aeMovieCompositionData * _composition, const LambdaResource & _lambda ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2DataInterface> Movie2DataInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}