#pragma once

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
		void setMovieInstance( const aeMovieInstance * _instance );

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

    protected:
        bool _isValid() const override;

    public:        
        bool storeResource_( const ResourcePtr & _resource );
        Resource * getResource_( const ae_string_t _name );

	public:
		Resource * createResourceImage_( const aeMovieResourceImage * _resource );
		Resource * createResourceVideo_( const aeMovieResourceVideo * _resource );
		Resource * createResourceSound_( const aeMovieResourceSound * _resource );
        Resource * createResourceParticle_( const aeMovieResourceParticle * _resource );

	protected:
        const aeMovieInstance * m_movieInstance;

		aeMovieData * m_movieData;

        FilePath m_filePath;

		typedef Vector<ResourcePtr> TVectorResources;
		TVectorResources m_resources;

        typedef Map<ConstString, ResourceMovie2Composition> TMapCompositions;
        TMapCompositions m_compositions;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ResourceMovie2> ResourceMovie2Ptr;
}