#pragma once

#include "AstralaxInterface.h"

#include "Interface/RenderSystemInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/String.h"

#include "AstralaxEmitterContainer.h"
#include "AstralaxEmitter2.h"

#include "Kernel/Factory.h"
#include "Kernel/Stringstream.h"
#include "Kernel/Vector.h"
#include "Kernel/Map.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#endif 

#include "magic.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop) 
#endif

namespace Mengine
{
    class AstralaxParticleSystem
        : public ServiceBase<AstralaxSystemInterface>
    {
    public:
        AstralaxParticleSystem();
        ~AstralaxParticleSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    public:
        AstralaxEmitterContainerInterfacePtr createEmitterContainerFromMemory( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ArchivatorInterfacePtr & _archivator, const DocumentPtr & _doc ) override;
        AstralaxEmitterInterfacePtr createEmitter( const AstralaxEmitterContainerInterfacePtr & _container, const DocumentPtr & _doc ) override;

    public:
        const RenderMaterialStage * getMaterialStage( int32_t _index ) const override;
        const ResourceImagePtr & getResourceImage( int32_t _index ) const override;

    public:
        uint32_t getEmitterCount() const override;

    public:
        bool updateAtlas();
        bool updateMaterial();

    protected:
        void onEmitterContainerRelease_( AstralaxEmitterContainer * _container );
        void onEmitterRelease_( AstralaxEmitter2 * _emitter );

    protected:
        ERenderPlatform m_renderPlatform;

        FactoryPtr m_factoryPoolAstralaxEmitterContainer;
        FactoryPtr m_factoryPoolAstralaxEmitter;

        struct AstralaxEmitterContainerDesc
        {
            uint32_t reference;
            const AstralaxEmitterContainer * container;
#ifdef MENGINE_DEBUG
            DocumentPtr doc;
#endif
        };

        typedef Map<uint32_t, AstralaxEmitterContainerDesc> MapHashEmitterContainers;
        MapHashEmitterContainers m_containers;

        int32_t m_materialCount;
        const RenderMaterialStage * m_stages[256] = {nullptr};

        typedef Vector<ResourceImagePtr> VectorAtlasDesc;
        VectorAtlasDesc m_atlases;

        struct MagicStatesCache
        {
            int32_t textures;
            MAGIC_TEXTURE_STATES states[MENGINE_RENDER_VERTEX_UV_COUNT];

            RenderFragmentShaderInterfacePtr fragmentShader;
        };

        typedef Vector<MagicStatesCache> VectorRenderFragmentShaderCache;
        VectorRenderFragmentShaderCache m_renderFragmentShaderCache;

    protected:
        void createFragmentShaderDX9Source_( Stringstream & ss, const MAGIC_MATERIAL * m );
        void createFragmentShaderGLSource_( Stringstream & ss, const MAGIC_MATERIAL * m );
        RenderFragmentShaderInterfacePtr cacheFragmentShader_( const MAGIC_MATERIAL * m );
    };
}