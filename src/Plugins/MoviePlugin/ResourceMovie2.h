#pragma once

#include "Interface/DataInterface.h"

#include "Movie2Interface.h"
#include "Movie2DataInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/BaseContent.h"
#include "Kernel/Viewport.h"
#include "Kernel/Color.h"
#include "Kernel/Map.h"

#include "math/mat4.h"

#include "movie/movie.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceMovie2
        : public Resource
        , protected BaseContent
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_CONTENTABLE();
        DECLARE_MAGICABLE( MAGIC_AEZ );

    public:
        ResourceMovie2();
        ~ResourceMovie2() override;

    public:
        bool hasComposition( const ConstString & _compositionName ) const;
        bool hasCompositionLayer( const ConstString & _compositionName, const ConstString & _layerName ) const;
        float getCompositionDuration( const ConstString & _compositionName ) const;
        float getCompositionFrameDuration( const ConstString & _compositionName ) const;

    public:
        const Movie2DataInterfacePtr & getData() const;
        const aeMovieCompositionData * getCompositionData( const ConstString & _compositionName ) const;

    public:
        struct CompositionLayer
        {
            uint32_t index;
            ConstString name;
            ConstString type;
            mt::mat4f matrix;
            Color color;
        };

        typedef Vector<CompositionLayer> VectorCompositionLayers;

        struct CompositionSubComposition
        {
            uint32_t index;
            ConstString name;

            float duration;
            float frameDuration;
        };

        typedef Vector<CompositionSubComposition> VectorCompositionSubCompositions;

        struct CompositionDesc
        {
            float duration;
            float frameDuration;

            bool has_bounds;
            mt::box2f bounds;

            bool master;

            VectorCompositionLayers layers;
            VectorCompositionSubCompositions subcompositions;
        };

    public:
        void setCompositionDesc( const ConstString & _name, const CompositionDesc & _composition );
        const CompositionDesc * getCompositionDesc( const ConstString & _compositionName ) const;

    public:
        typedef Lambda<void( const ConstString &, const CompositionDesc & )> LambdaCompositionDescs;
        void foreachCompositionDesc( const LambdaCompositionDescs & _lambda );

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        Movie2DataInterfacePtr m_data;

        typedef Map<ConstString, CompositionDesc> MapCompositions;
        MapCompositions m_compositions;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceMovie2> ResourceMovie2Ptr;
    //////////////////////////////////////////////////////////////////////////
}