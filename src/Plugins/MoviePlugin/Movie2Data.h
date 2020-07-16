#pragma once

#include "Interface/RenderMaterialInterface.h"

#include "Movie2DataInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Pool.h"

namespace Mengine
{
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

    public:
        bool acquireCompositionData( const aeMovieCompositionData * _compositionData ) override;
        void releaseCompositionData( const aeMovieCompositionData * _compositionData ) override;

    public:
        void visitCompositionDataResources( const aeMovieCompositionData * _compositionData, const LambdaResource & _lambda ) override;

    public:
        void setMovieData( const aeMovieData * _movieData );
        const aeMovieData * getMovieData() const override;

    public:
        const ResourcePtr & getResource( const ae_string_t _resourceName );

    public:
        struct ImageDesc
        {
            uint32_t refcount;

            ResourceImagePtr resourceImage;
            RenderMaterialInterfacePtr materials[4];
        };

    public:
        ImageDesc * makeImageDesc( const ResourceImagePtr & _resource );
        void removeImageDesc( ImageDesc * _desc );

    protected:
        const aeMovieData * m_movieData;

        typedef Pool<ImageDesc, 512> PoolImageDesc;
        PoolImageDesc m_poolImageDesc;

        typedef Vector<ImageDesc *> VectorImageDesc;
        VectorImageDesc m_images;

        typedef Vector<ResourcePtr> VectorResources;
        VectorResources m_resources;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2Data> Movie2DataPtr;
    //////////////////////////////////////////////////////////////////////////
}