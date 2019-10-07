#pragma once

#include "Interface/RenderMaterialInterface.h"

#include "Movie2DataInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Pool.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct Movie2DataImageDesc
    {
        ResourceImagePtr resourceImage;

        RenderMaterialInterfacePtr materials[4];
    };
    //////////////////////////////////////////////////////////////////////////
    class Movie2Data
        : public Movie2DataInterface
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
        const ResourcePtr & getResource( const ae_string_t _resourceName );

        Movie2DataImageDesc * makeImageDesc( const ResourceImagePtr & _resource );
        void removeImageDesc( Movie2DataImageDesc * _desc );

    protected:
        const aeMovieData * m_movieData;

        typedef Pool<Movie2DataImageDesc, 512> PoolImageDesc;
        PoolImageDesc m_poolImageDesc;

        typedef Vector<Movie2DataImageDesc *> VectorImageDesc;
        VectorImageDesc m_images;

        typedef Vector<ResourcePtr> VectorResources;
        VectorResources m_resources;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2Data> Movie2DataPtr;
    //////////////////////////////////////////////////////////////////////////
}