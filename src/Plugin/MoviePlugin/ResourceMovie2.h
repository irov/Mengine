#pragma once

#include "Interface/ArchiveInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/ColourValue.h"

#include "math/mat4.h"

#include "movie/movie.hpp"

#include "Config/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct ResourceMovie2CompositionLayer
    {
        uint32_t index;
        ConstString name;
        ConstString type;
        mt::mat4f matrix;
        ColourValue color;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ResourceMovie2CompositionLayer> ResourceMovie2CompositionLayers;
    //////////////////////////////////////////////////////////////////////////
    struct ResourceMovie2CompositionSubComposition
    {
        uint32_t index;
        ConstString name;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ResourceMovie2CompositionSubComposition> ResourceMovie2CompositionSubCompositions;
    //////////////////////////////////////////////////////////////////////////
    struct ResourceMovie2Composition
    {
        float duration;
        float frameDuration;

        ResourceMovie2CompositionLayers layers;
        ResourceMovie2CompositionSubCompositions subcompositions;
    };
    //////////////////////////////////////////////////////////////////////////
    class ResourceMovie2
        : public Resource
    {
        DECLARE_VISITABLE( Resource );

    public:
        ResourceMovie2();
        ~ResourceMovie2() override;

    public:
        void setMovieInstance( const aeMovieInstance * _movieInstance );
        const aeMovieInstance * getMovieInstance() const;

    public:
        void setMovieArchivator( const ArchivatorInterfacePtr & _movieArchivator );
        const ArchivatorInterfacePtr & ResourceMovie2::getMovieArchivator() const;

    public:
        void setFilePath( const FilePath & _filePath );
        const FilePath & getFilePath() const;

    public:
        bool hasComposition( const ConstString & _name ) const;
        float getCompositionDuration( const ConstString & _name ) const;
        float getCompositionFrameDuration( const ConstString & _name ) const;

    public:
        const aeMovieData * getMovieData() const;
        const aeMovieCompositionData * getCompositionData( const ConstString & _name ) const;

    public:
        const ResourceMovie2Composition * getCompositionDesc( const ConstString & _name ) const;

    protected:
        bool _loader( const Metabuf::Metadata * _parser ) override;

    protected:
        bool _compile() override;
        void _release() override;

    public:
        bool storeResource_( const ResourcePtr & _resource );
        Resource * getResource_( const ae_string_t _name );

    protected:
        const aeMovieInstance * m_movieInstance;

        ArchivatorInterfacePtr m_movieArchivator;

        const aeMovieData * m_movieData;

        FilePath m_filePath;

        typedef Vector<ResourcePtr> VectorResources;
        VectorResources m_resources;

        typedef Map<ConstString, ResourceMovie2Composition> MapCompositions;
        MapCompositions m_compositions;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceMovie2, Resource> ResourceMovie2Ptr;
    //////////////////////////////////////////////////////////////////////////
}