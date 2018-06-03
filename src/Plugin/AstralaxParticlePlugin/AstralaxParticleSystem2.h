#pragma once

#include "Interface/ParticleSystemInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "Core/ServiceBase.h"

#include "Config/String.h"

#include "AstralaxEmitterContainer2.h"
#include "AstralaxEmitter2.h"

#include "Factory/Factory.h"

#include "Config/Stringstream.h"
#include "Config/Vector.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(push, 0) 
#endif 

#include "magic.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(pop) 
#endif

#include "Config/Map.h"

namespace Mengine
{
    class AstralaxParticleSystem2
        : public ServiceBase<ParticleSystemInterface2>
    {
    public:
        AstralaxParticleSystem2();
        ~AstralaxParticleSystem2() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:        
        ParticleEmitterContainerInterface2Ptr createEmitterContainerFromMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, const ConstString & _whoName ) override;
        ParticleEmitterInterfacePtr createEmitter( const ParticleEmitterContainerInterface2Ptr & _container ) override;

    public:
        const RenderMaterialStage * getMaterialStage( int _index ) const override;
        const ResourceImagePtr & getResourceImage( int _index ) const override;

    public:
        uint32_t getEmitterCount() const override;

    public:
        bool updateAtlas();
        bool updateMaterial();

    protected:
        void onEmitterContainerRelease_( AstralaxEmitterContainer2 * _container );
        void onEmitterRelease_( AstralaxEmitter2 * _emitter );

    protected:
        ERenderPlatform m_renderPlatform;

        FactoryPtr m_factoryPoolAstralaxEmitterContainer;
        FactoryPtr m_factoryPoolAstralaxEmitter;
        
        struct AstralaxEmitterContainerDesc
        {
            uint32_t reference;
            const AstralaxEmitterContainer2 * container;
        };

        typedef Map<uint32_t, AstralaxEmitterContainerDesc> TMapHashEmitterContainers;
        TMapHashEmitterContainers m_containers;

        int m_materialCount;
        const RenderMaterialStage * m_stages[256];

        typedef Vector<ResourceImagePtr> TVectorAtlasDesc;
        TVectorAtlasDesc m_atlases;

        struct MagicStatesCache
        {
            int textures;
            MAGIC_TEXTURE_STATES states[MENGINE_RENDER_VERTEX_UV_COUNT];

            RenderFragmentShaderInterfacePtr fragmentShader;
        };

        typedef Vector<MagicStatesCache> TVectorRenderFragmentShaderCache;
        TVectorRenderFragmentShaderCache m_renderFragmentShaderCache;

    protected:
        void createFragmentShaderDX9Source_( Stringstream & ss, const MAGIC_MATERIAL * m );
        void createFragmentShaderGLSource_( Stringstream & ss, const MAGIC_MATERIAL * m );
        RenderFragmentShaderInterfacePtr cacheFragmentShader_( const MAGIC_MATERIAL * m );
	};
}