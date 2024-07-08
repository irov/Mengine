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
        DECLARE_FACTORABLE( Movie2Data );

    public:
        Movie2Data();
        ~Movie2Data() override;

    public:
        void setGroupName( const ConstString & _groupName ) override;
        const ConstString & getGroupName() const override;

    public:
        bool acquire() override;
        void release() override;

    public:
        bool acquireCompositionData( const aeMovieCompositionData * _compositionData ) override;
        void releaseCompositionData( const aeMovieCompositionData * _compositionData ) override;

    public:
        void foreachCompositionDataResources( const aeMovieCompositionData * _compositionData, const LambdaCompositionDataResource & _lambda ) override;

    public:
        void setMovieData( const aeMovieData * _movieData );
        const aeMovieData * getMovieData() const override;

    public:
        const ResourcePtr & getResource( const Char * _resourceName );

    public:
        struct ImageDesc
        {
            uint32_t refcount;

            ResourceImage * resourceImage;
            RenderMaterialInterfacePtr materials[4];
        };

    public:
        ImageDesc * makeImageDesc( const ResourcePtr & _resource );
        void removeImageDesc( ImageDesc * _desc );

    protected:
        const aeMovieData * m_movieData;

        ConstString m_groupName;

        typedef Pool<ImageDesc, 512> PoolImageDesc;
        PoolImageDesc m_poolImageDesc;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2Data, Movie2DataInterface> Movie2DataPtr;
    //////////////////////////////////////////////////////////////////////////
}