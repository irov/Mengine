#pragma once

#include "Interface/RenderSystemInterface.h"

#include "AstralaxInterface.h"
#include "AstralaxIncluder.h"
#include "AstralaxEmitterContainer.h"
#include "AstralaxEmitter2.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/String.h"
#include "Kernel/Factory.h"
#include "Kernel/Stringstream.h"
#include "Kernel/Vector.h"
#include "Kernel/Map.h"

namespace Mengine
{
    class AstralaxService
        : public ServiceBase<AstralaxServiceInterface>
    {
    public:
        AstralaxService();
        ~AstralaxService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    public:
        AstralaxEmitterContainerInterfacePtr createEmitterContainerFromFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc ) override;

    public:
        AstralaxEmitterInterfacePtr createEmitter( const AstralaxEmitterContainerInterfacePtr & _container, const DocumentPtr & _doc ) override;

    public:
        const RenderMaterialInterfacePtr & getMaterial( const AstralaxMesh & _mesh ) const override;

    protected:
        const RenderMaterialStage * getMaterialStage( int32_t _index ) const;
        const ResourceImagePtr & getResourceImage( int32_t _index ) const;

    public:
        uint32_t getEmitterCount() const override;
        uint32_t getMaxParticlesCount() const override;

    public:
        bool updateAtlas() override;

    public:
        bool updateMaterial();

    protected:
        void onEmitterContainerRelease_( AstralaxEmitterContainer * _container );
        void onEmitterRelease_( AstralaxEmitter2 * _emitter );

    protected:
        ArchivatorInterfacePtr m_archivator;

        uint32_t m_maxParticlesNum;

        ERenderPlatform m_renderPlatform;

        FactoryPtr m_factoryPoolAstralaxEmitterContainer;
        FactoryPtr m_factoryPoolAstralaxEmitter;

        struct AstralaxEmitterContainerDesc
        {
            uint32_t reference;
            const AstralaxEmitterContainer * container;

#ifdef MENGINE_DEBUG
            ConstString fileGroupName;
            FilePath FilePath;
#endif

#ifdef MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc;
#endif
        };

        typedef Map<uint32_t, AstralaxEmitterContainerDesc> MapHashEmitterContainers;
        MapHashEmitterContainers m_containers;

        int32_t m_stageCount;
        const RenderMaterialStage * m_stages[256];

        typedef Vector<ResourceImagePtr> VectorAtlasDesc;
        VectorAtlasDesc m_atlases;

        struct MagicMaterialDesc
        {
            const RenderMaterialStage * stage;

            ResourceImagePtr images[MENGINE_MAX_TEXTURE_STAGES];
            uint32_t imageCount;

            RenderMaterialInterfacePtr material;
        };

        typedef Vector<MagicMaterialDesc> VectorMaterialDesc;
        mutable VectorMaterialDesc m_materials;

        struct MagicStatesCache
        {
            int32_t textures;
            MAGIC_TEXTURE_STATES states[MENGINE_RENDER_VERTEX_UV_COUNT];

            RenderFragmentShaderInterfacePtr fragmentShader;
        };

        typedef Vector<MagicStatesCache> VectorRenderFragmentShaderCache;
        mutable VectorRenderFragmentShaderCache m_renderFragmentShaderCache;

    protected:
        void createFragmentShaderDX9Source_( Stringstream & ss, const MAGIC_MATERIAL * m ) const;
        void createFragmentShaderDX11Source_( Stringstream & ss, const MAGIC_MATERIAL * m ) const;
        void createFragmentShaderGLSource_( Stringstream & ss, const MAGIC_MATERIAL * m ) const;
        void createFragmentShaderGLESSource_( Stringstream & ss, const MAGIC_MATERIAL * m ) const;

        RenderFragmentShaderInterfacePtr cacheFragmentShader_( const MAGIC_MATERIAL * m ) const;
    };
}