#pragma once

#include "Movie2DataInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie2Data
        : public Movie2DataInterface
        , public Factorable
    {
    public:
        Movie2Data();
        ~Movie2Data() override;

    public:
        bool acquire() override;
        void release() override;

    protected:
        Pointer allocateMemory( size_t _size ) const override;

    public:
        void setMovieData( const aeMovieData * _movieData );
        const aeMovieData * getMovieData() const override;

    public:
        Resource * getResource( const ae_string_t _resourceName );

    protected:
        const aeMovieData * m_movieData;

        typedef Vector<ResourcePtr> VectorResources;
        VectorResources m_resources;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2Data> Movie2DataPtr;
    //////////////////////////////////////////////////////////////////////////
}