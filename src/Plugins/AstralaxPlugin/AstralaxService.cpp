#include "AstralaxService.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionContainer.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/ConstStringHelper.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "Config/StdString.h"
#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AstralaxService, Mengine::AstralaxService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AstralaxService::AstralaxService()
        : m_maxParticlesNum( 0 )
        , m_renderPlatform( RP_UNKNOWN )
        , m_stageCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxService::~AstralaxService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxService::_initializeService()
    {
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        m_maxParticlesNum = CONFIG_VALUE( "Engine", "ParticleMaxCount", 10000U );

        bool states[17];
        states[MAGIC_RENDER_STATE_BLENDING] = false;
        states[MAGIC_RENDER_STATE_TEXTURE_COUNT] = false;
        states[MAGIC_RENDER_STATE_TEXTURE] = true;
        states[MAGIC_RENDER_STATE_ADDRESS_U] = false;
        states[MAGIC_RENDER_STATE_ADDRESS_V] = false;
        states[MAGIC_RENDER_STATE_OPERATION_RGB] = false;
        states[MAGIC_RENDER_STATE_ARGUMENT1_RGB] = false;
        states[MAGIC_RENDER_STATE_ARGUMENT2_RGB] = false;
        states[MAGIC_RENDER_STATE_OPERATION_ALPHA] = false;
        states[MAGIC_RENDER_STATE_ARGUMENT1_ALPHA] = false;
        states[MAGIC_RENDER_STATE_ARGUMENT2_ALPHA] = false;
        states[MAGIC_RENDER_STATE_ZENABLE] = false;
        states[MAGIC_RENDER_STATE_ZWRITE] = false;
        states[MAGIC_RENDER_STATE_ALPHATEST_INIT] = false;
        states[MAGIC_RENDER_STATE_ALPHATEST] = false;
        states[MAGIC_RENDER_STATE_TECHNIQUE_ON] = false;
        states[MAGIC_RENDER_STATE_TECHNIQUE_OFF] = false;

        Magic_SetRenderStateFilter( states, false );

        Magic_EnableZBuffer( false );

        Magic_SetAxis( MAGIC_pXnYpZ );

        const char * version = Magic_GetVersion();

        if( MENGINE_STRSTR( version, MAGIC_API ) == nullptr )
        {
            LOGGER_ERROR( "Astralax Magic Particles Version: '%s' but need '%s'"
                , version
                , MAGIC_API
            );

            return false;
        }

        LOGGER_MESSAGE( "Astralax Magic Particles Version: %s"
            , version
        );

        if( SERVICE_EXIST( RenderSystemInterface ) == true )
        {
            m_renderPlatform = RENDER_SYSTEM()
                ->getRenderPlatformType();
        }

        m_factoryPoolAstralaxEmitterContainer = Helper::makeFactoryPoolWithListener<AstralaxEmitterContainer, 16>( this, &AstralaxService::onEmitterContainerRelease_, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryPoolAstralaxEmitter = Helper::makeFactoryPoolWithListener<AstralaxEmitter2, 16>( this, &AstralaxService::onEmitterRelease_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxService::_finalizeService()
    {
        MENGINE_ASSERTION_CONTAINER_EMPTY( m_containers );

        m_containers.clear();

        m_archivator = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPoolAstralaxEmitterContainer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPoolAstralaxEmitter );

        m_factoryPoolAstralaxEmitterContainer = nullptr;
        m_factoryPoolAstralaxEmitter = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxService::_stopService()
    {
        this->updateAtlas();

        for( const ResourceImagePtr & resourceImage : m_atlases )
        {
            resourceImage->release();
        }

        m_atlases.clear();

        for( uint32_t index = 0; index != 256; ++index )
        {
            const RenderMaterialStage * stage = m_stages[index];

            RENDERMATERIAL_SERVICE()
                ->uncacheMaterialStage( stage );
        }

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_materials );

        m_materials.clear();

        m_renderFragmentShaderCache.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitterContainerInterfacePtr AstralaxService::createEmitterContainerFromFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc )
    {
        AstralaxEmitterContainerPtr container = m_factoryPoolAstralaxEmitterContainer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( container, "invalid create container doc '%s'"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( container->initialize( _fileGroup, _filePath, m_archivator ) == false )
        {
            LOGGER_ERROR( "invalid initialize container (doc: %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        uint32_t id = container->getPtcId();

        if( id == 0 )
        {
            LOGGER_ERROR( "invalid container ptc id (doc: %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        MapHashEmitterContainers::iterator it_found = m_containers.find( id );

        if( it_found == m_containers.end() )
        {
            AstralaxEmitterContainerDesc new_desc;
            new_desc.reference = 0;
            new_desc.container = container.get();
#if MENGINE_DOCUMENT_ENABLE
            new_desc.doc = _doc;
#endif

            it_found = m_containers.emplace( id, new_desc ).first;
        }
        else
        {
            container->finalize();

#if MENGINE_DOCUMENT_ENABLE
            const AstralaxEmitterContainerDesc & old_desc = it_found->second;

            LOGGER_PERFORMANCE( "useless load container '%s' original is '%s'"
                , MENGINE_DOCUMENT_STR( _doc )
                , MENGINE_DOCUMENT_STR( old_desc.doc )
            );
#endif
        }

        AstralaxEmitterContainerDesc & desc = it_found->second;
        ++desc.reference;

        AstralaxEmitterContainerPtr new_container( desc.container );

        return new_container;
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitterInterfacePtr AstralaxService::createEmitter( const AstralaxEmitterContainerInterfacePtr & _container, const DocumentPtr & _doc )
    {
        AstralaxEmitter2Ptr emitter = m_factoryPoolAstralaxEmitter->createObject( _doc );

        if( emitter->initialize( _container ) == false )
        {
            return nullptr;
        }

        if( this->updateMaterial() == false )
        {
            return nullptr;
        }

        if( this->updateAtlas() == false )
        {
            return nullptr;
        }

        return emitter;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxService::updateAtlas()
    {
        MAGIC_CHANGE_ATLAS c;
        while( Magic_GetNextAtlasChange( &c ) == MAGIC_SUCCESS )
        {
            switch( c.type )
            {
            case MAGIC_CHANGE_ATLAS_LOAD:
                {
                }break;
            case MAGIC_CHANGE_ATLAS_CLEAN:
                {
                }break;
            case MAGIC_CHANGE_ATLAS_CREATE:
                {
                    MapHashEmitterContainers::const_iterator it_found = m_containers.find( c.ptc_id );

                    if( it_found == m_containers.end() )
                    {
                        return false;
                    }

                    const AstralaxEmitterContainerDesc & desc = it_found->second;

                    const ResourceImagePtr & resourceImage = desc.container->getAtlasResourceImage( c.file );

                    if( resourceImage->compile() == false )
                    {
                        return false;
                    }

                    m_atlases.emplace_back( resourceImage );
                }break;
            case MAGIC_CHANGE_ATLAS_DELETE:
                {
                    VectorAtlasDesc::iterator it_remove = m_atlases.begin();
                    std::advance( it_remove, c.index );

                    const ResourceImagePtr & resourceImage = *it_remove;

                    resourceImage->release();

                    m_materials.erase( std::remove_if( m_materials.begin(), m_materials.end(), [resourceImage]( const MagicMaterialDesc & _desc )
                    {
                        for( uint32_t index = 0; index != _desc.imageCount; ++index )
                        {
                            if( _desc.images[index] == resourceImage )
                            {
                                return true;
                            }
                        }

                        return false;
                    } ), m_materials.end() );

                    m_atlases.erase( it_remove );
                }break;
            default:
                LOGGER_ERROR( "invalid atlas state"
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxService::updateMaterial()
    {
        int32_t newMaterialCount = Magic_GetMaterialCount();

        for( int32_t i = m_stageCount; i != newMaterialCount; ++i )
        {
            MAGIC_MATERIAL m;
            if( Magic_GetMaterial( i, &m ) != MAGIC_SUCCESS )
            {
                LOGGER_ERROR( "invalid get material %d"
                    , i
                );

                return false;
            }

            RenderMaterialStage rs;

            switch( m.blending )
            {
            case MAGIC_BLENDING_NORMAL:
                {
                    rs.alphaBlendEnable = true;
                    rs.blendSrc = BF_SOURCE_ALPHA;
                    rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;
                }break;
            case MAGIC_BLENDING_ADD:
                {
                    rs.alphaBlendEnable = true;
                    rs.blendSrc = BF_SOURCE_ALPHA;
                    rs.blendDst = BF_ONE;
                }break;
            case MAGIC_BLENDING_OPACITY:
                {
                    rs.alphaBlendEnable = false;
                }break;
            }

            RenderVertexAttributeInterfacePtr vertexAttribute;
            RenderVertexShaderInterfacePtr vertexShader;

            switch( m.textures )
            {
            case 0:
                {
                    vertexAttribute = RENDERMATERIAL_SERVICE()
                        ->getVertexAttribute( STRINGIZE_STRING_LOCAL( "Vertex2D_UV0" ) );

                    vertexShader = RENDERMATERIAL_SERVICE()
                        ->getVertexShader( STRINGIZE_STRING_LOCAL( "Vertex_Color" ) );
                }break;
            case 1:
                {
                    vertexAttribute = RENDERMATERIAL_SERVICE()
                        ->getVertexAttribute( STRINGIZE_STRING_LOCAL( "Vertex2D_UV1" ) );

                    vertexShader = RENDERMATERIAL_SERVICE()
                        ->getVertexShader( STRINGIZE_STRING_LOCAL( "Vertex_Blend" ) );
                }break;
            case 2:
                {
                    vertexAttribute = RENDERMATERIAL_SERVICE()
                        ->getVertexAttribute( STRINGIZE_STRING_LOCAL( "Vertex2D_UV2" ) );

                    vertexShader = RENDERMATERIAL_SERVICE()
                        ->getVertexShader( STRINGIZE_STRING_LOCAL( "Vertex_Blend_ExternalAlpha" ) );
                }break;
            default:
                return false;
                break;
            };

            MENGINE_ASSERTION_MEMORY_PANIC( vertexAttribute );
            MENGINE_ASSERTION_MEMORY_PANIC( vertexShader );

            RenderFragmentShaderInterfacePtr fragmentShader = this->cacheFragmentShader_( &m );

            MENGINE_ASSERTION_MEMORY_PANIC( fragmentShader );

            RenderProgramInterfacePtr program = RENDER_SYSTEM()
                ->createProgram( STRINGIZE_STRING_LOCAL( "AstralaxProgram" ), vertexShader, fragmentShader, vertexAttribute, m.textures, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( program );

            rs.program = program;

            for( int32_t stage = 0; stage != m.textures; ++stage )
            {
                const MAGIC_TEXTURE_STATES & state = m.states[stage];

                const ETextureAddressMode dx_address[] = {TAM_WRAP, TAM_MIRROR, TAM_CLAMP, TAM_BORDER};

                RenderTextureStage & textureStage = rs.textureStages[stage];

                textureStage.mipmap = RENDERMATERIAL_SERVICE()
                    ->getDefaultTextureFilterMipmap();

                textureStage.magnification = RENDERMATERIAL_SERVICE()
                    ->getDefaultTextureFilterMagnification();

                textureStage.minification = RENDERMATERIAL_SERVICE()
                    ->getDefaultTextureFilterMinification();

                textureStage.addressU = dx_address[state.address_u];
                textureStage.addressV = dx_address[state.address_v];
                textureStage.addressBorder = 0x00000000;
            }

            const RenderMaterialStage * cache_stage = RENDERMATERIAL_SERVICE()
                ->cacheMaterialStage( rs, MENGINE_DOCUMENT_FACTORABLE );

            m_stages[i] = cache_stage;
        }

        m_stageCount = newMaterialCount;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialStage * AstralaxService::getMaterialStage( int32_t _index ) const
    {
        if( _index >= m_stageCount )
        {
            return nullptr;
        }

        return m_stages[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & AstralaxService::getResourceImage( int32_t _index ) const
    {
        MENGINE_ASSERTION_FATAL( (VectorAtlasDesc::size_type)_index < m_atlases.size(), "index %d but size is %u"
            , _index
            , (uint32_t)m_atlases.size()
        );

        const ResourceImagePtr & resourceImage = m_atlases[_index];

        return resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialInterfacePtr & AstralaxService::getMaterial( const AstralaxMesh & _mesh ) const
    {
        if( _mesh.textures >= MENGINE_MAX_TEXTURE_STAGES )
        {
            return RenderMaterialInterfacePtr::none();
        }

        const RenderMaterialStage * stage = this->getMaterialStage( _mesh.material );

        for( const MagicMaterialDesc & desc : m_materials )
        {
            if( desc.stage != stage )
            {
                continue;
            }

            if( desc.imageCount != _mesh.textures )
            {
                continue;
            }

            bool test_textures = true;
            for( uint32_t index = 0; index != _mesh.textures; ++index )
            {
                int32_t textureId = _mesh.texture[index];

                const ResourceImagePtr & image = this->getResourceImage( textureId );

                MENGINE_ASSERTION_MEMORY_PANIC( image );

                if( desc.images[index] != image )
                {
                    test_textures = false;
                    break;
                }
            }

            if( test_textures == false )
            {
                continue;
            }

            return desc.material;
        }

        MagicMaterialDesc desc;

        desc.stage = stage;

        RenderTextureInterfacePtr textures[MENGINE_MAX_TEXTURE_STAGES];

        for( uint32_t index = 0; index != _mesh.textures; ++index )
        {
            int32_t textureId = _mesh.texture[index];

            const ResourceImagePtr & image = this->getResourceImage( textureId );

            MENGINE_ASSERTION_MEMORY_PANIC( image );

            desc.images[index] = image;

            const RenderTextureInterfacePtr & texture = image->getTexture();

            MENGINE_ASSERTION_MEMORY_PANIC( texture );

            textures[index] = texture;
        }

        desc.imageCount = _mesh.textures;

        const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
            ->getMaterial2( STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), stage, PT_TRIANGLELIST, textures, desc.imageCount, MENGINE_DOCUMENT_FORWARD );

        desc.material = material;

        const MagicMaterialDesc & emplace_desc = m_materials.emplace_back( desc );

        const RenderMaterialInterfacePtr & new_material = emplace_desc.material;

        return new_material;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AstralaxService::getEmitterCount() const
    {
        uint32_t countObject = m_factoryPoolAstralaxEmitter->getCountObject();

        return countObject;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AstralaxService::getMaxParticlesCount() const
    {
        return m_maxParticlesNum;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxService::createFragmentShaderDX9Source_( Stringstream & ss, const MAGIC_MATERIAL * m )
    {
        int32_t textures = m->textures;

        for( int32_t i = 0; i != textures; ++i )
        {
            ss << "sampler2D tex" << i << ";" << std::endl;
        }

        ss << std::endl;

        ss << "struct v2p {" << std::endl;
        ss << "  float4 position : POSITION;" << std::endl;
        ss << "  float4 color : COLOR0;" << std::endl;
        ss << "  float2 uv0 : TEXCOORD0;" << std::endl;
        ss << "  float2 uv1 : TEXCOORD1;" << std::endl;
        ss << "  float2 uv2 : TEXCOORD2;" << std::endl;
        ss << "  float2 uv3 : TEXCOORD3;" << std::endl;
        ss << "};" << std::endl;

        ss << std::endl;

        ss << "struct p2f {" << std::endl;
        ss << "  float4 color : COLOR0;" << std::endl;
        ss << "};" << std::endl;

        ss << std::endl;

        ss << "void main( in v2p IN, out p2f OUT )" << std::endl;
        ss << "{" << std::endl;

        if( textures != 0 )
        {
            ss << "  float4 color;" << std::endl;
            ss << "  float4 arg1;" << std::endl;
            ss << "  float4 arg2;" << std::endl;
            ss << "  float4 colorTex;" << std::endl;
            ss << "  float4 colorVarying=IN.color;" << std::endl;

            for( int32_t i = 0; i != textures; ++i )
            {
                ss << std::endl;

                MAGIC_TEXTURE_STATES * s = m->states + i;

                if( s->argument_rgb1 == MAGIC_TEXARG_TEXTURE ||
                    s->argument_alpha1 == MAGIC_TEXARG_TEXTURE ||
                    (s->operation_rgb != MAGIC_TEXOP_ARGUMENT1 && s->argument_rgb2 == MAGIC_TEXARG_TEXTURE) ||
                    (s->operation_alpha != MAGIC_TEXOP_ARGUMENT1 && s->argument_alpha2 == MAGIC_TEXARG_TEXTURE) )
                {
                    ss << "  colorTex = tex2D(tex" << i << ", IN.uv" << i << ");" << std::endl;
                }

                if( s->argument_rgb1 == s->argument_alpha1 )
                {
                    switch( s->argument_rgb1 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg1 = color;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg1 = colorVarying;" << std::endl;
                        break;
                    default:
                        ss << "  arg1 = colorTex;" << std::endl;
                        break;
                    }
                }
                else
                {
                    switch( s->argument_rgb1 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg1.xyz = color.xyz;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg1.xyz = colorVarying.xyz;" << std::endl;
                        break;
                    default:
                        ss << "  arg1.xyz = colorTex.xyz;" << std::endl;
                        break;
                    }

                    switch( s->argument_alpha1 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg1.w = color.w;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg1.w = colorVarying.w;" << std::endl;
                        break;
                    default:
                        ss << "  arg1.w = colorTex.w;" << std::endl;
                        break;
                    }
                }

                if( s->argument_rgb2 == s->argument_alpha2 && s->operation_rgb != MAGIC_TEXOP_ARGUMENT1 && s->operation_alpha != MAGIC_TEXOP_ARGUMENT1 )
                {
                    switch( s->argument_rgb2 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg2 = color;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg2 = colorVarying;" << std::endl;
                        break;
                    default:
                        ss << "  arg2 = colorTex;" << std::endl;
                        break;
                    }
                }
                else
                {
                    if( s->operation_rgb != MAGIC_TEXOP_ARGUMENT1 )
                    {
                        switch( s->argument_rgb2 )
                        {
                        case MAGIC_TEXARG_CURRENT:
                            ss << "  arg2.xyz = color.xyz;" << std::endl;
                            break;
                        case MAGIC_TEXARG_DIFFUSE:
                            ss << "  arg2.xyz = colorVarying.xyz;" << std::endl;
                            break;
                        default:
                            ss << "  arg2.xyz = colorTex.xyz;" << std::endl;
                            break;
                        }
                    }
                    if( s->operation_alpha != MAGIC_TEXOP_ARGUMENT1 )
                    {
                        switch( s->argument_alpha2 )
                        {
                        case MAGIC_TEXARG_CURRENT:
                            ss << "  arg2.w = color.w;" << std::endl;;
                            break;
                        case MAGIC_TEXARG_DIFFUSE:
                            ss << "  arg2.w = colorVarying.w;" << std::endl;;
                            break;
                        default:
                            ss << "  arg2.w = colorTex.w;" << std::endl;;
                            break;
                        }
                    }
                }

                if( s->operation_rgb == s->operation_alpha )
                {
                    switch( s->operation_rgb )
                    {
                    case MAGIC_TEXOP_ARGUMENT1:
                        ss << "  color = arg1;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_ADD:
                        ss << "  color = arg1 + arg2;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_SUBTRACT:
                        ss << "  color = arg1 - arg2;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE:
                        ss << "  color = arg1 * arg2;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE2X:
                        ss << "  color = arg1 * arg2;" << std::endl;;
                        ss << "  color = 2 * color;" << std::endl;;
                        break;
                    default:
                        ss << "  color = arg1 * arg2;" << std::endl;;
                        ss << "  color = 4 * color;" << std::endl;;
                        break;
                    }
                }
                else
                {
                    switch( s->operation_rgb )
                    {
                    case MAGIC_TEXOP_ARGUMENT1:
                        ss << "  color.xyz = arg1.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_ADD:
                        ss << "  color.xyz = arg1.xyz + arg2.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_SUBTRACT:
                        ss << "  color.xyz = arg1.xyz - arg2.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE:
                        ss << "  color.xyz = arg1.xyz * arg2.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE2X:
                        ss << "  color.xyz = arg1.xyz * arg2.xyz;" << std::endl;;
                        ss << "  color.xyz = 2 * color.xyz;" << std::endl;;
                        break;
                    default:
                        ss << "  color.xyz = arg1.xyz * arg2.xyz;" << std::endl;;
                        ss << "  color.xyz = 4 * color.xyz;" << std::endl;;
                        break;
                    }

                    switch( s->operation_alpha )
                    {
                    case MAGIC_TEXOP_ARGUMENT1:
                        ss << "  color.w = arg1.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_ADD:
                        ss << "  color.w = arg1.w + arg2.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_SUBTRACT:
                        ss << "  color.w = arg1.w - arg2.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE:
                        ss << "  color.w = arg1.w * arg2.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE2X:
                        ss << "  color.w = arg1.w * arg2.w;" << std::endl;;
                        ss << "  color.w = 2 * color.w;" << std::endl;;
                        break;
                    default:
                        ss << "  color.w = arg1.w * arg2.w;" << std::endl;;
                        ss << "  color.w = 4 * color.w;" << std::endl;;
                        break;
                    }
                }
            }

            ss << "  OUT.color = IN.color * color;" << std::endl;
        }
        else
        {
            ss << "  OUT.color = IN.color;" << std::endl;
        }

        ss << "}" << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxService::createFragmentShaderDX11Source_( Stringstream & ss, const MAGIC_MATERIAL * m )
    {
        int32_t textures = m->textures;

        for( int32_t i = 0; i != textures; ++i )
        {
            ss << "Texture2D tex" << i << " : register(t" << i << ");" << std::endl;
        }

        for( int32_t i = 0; i != textures; ++i )
        {
            ss << "SamplerState sampler" << i << " : register(s" << i << ");" << std::endl;
        }

        ss << std::endl;

        ss << "struct v2p {" << std::endl;
        ss << "  float4 position : SV_POSITION;" << std::endl;
        ss << "  float4 color : COLOR0;" << std::endl;
        ss << "  float2 uv0 : TEXCOORD0;" << std::endl;
        ss << "  float2 uv1 : TEXCOORD1;" << std::endl;
        ss << "  float2 uv2 : TEXCOORD2;" << std::endl;
        ss << "  float2 uv3 : TEXCOORD3;" << std::endl;
        ss << "};" << std::endl;

        ss << std::endl;

        ss << "struct p2f {" << std::endl;
        ss << "  float4 color : COLOR0;" << std::endl;
        ss << "};" << std::endl;

        ss << std::endl;

        ss << "void main( in v2p IN, out p2f OUT )" << std::endl;
        ss << "{" << std::endl;

        if( textures != 0 )
        {
            ss << "  float4 color;" << std::endl;
            ss << "  float4 arg1;" << std::endl;
            ss << "  float4 arg2;" << std::endl;
            ss << "  float4 colorTex;" << std::endl;
            ss << "  float4 colorVarying=IN.color;" << std::endl;

            for( int32_t i = 0; i != textures; ++i )
            {
                ss << std::endl;

                MAGIC_TEXTURE_STATES * s = m->states + i;

                if( s->argument_rgb1 == MAGIC_TEXARG_TEXTURE ||
                    s->argument_alpha1 == MAGIC_TEXARG_TEXTURE ||
                    (s->operation_rgb != MAGIC_TEXOP_ARGUMENT1 && s->argument_rgb2 == MAGIC_TEXARG_TEXTURE) ||
                    (s->operation_alpha != MAGIC_TEXOP_ARGUMENT1 && s->argument_alpha2 == MAGIC_TEXARG_TEXTURE) )
                {
                    ss << "  colorTex = tex" << i << ".Sample( sampler" << i << ", IN.uv" << i << ");" << std::endl;
                }

                if( s->argument_rgb1 == s->argument_alpha1 )
                {
                    switch( s->argument_rgb1 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg1 = color;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg1 = colorVarying;" << std::endl;
                        break;
                    default:
                        ss << "  arg1 = colorTex;" << std::endl;
                        break;
                    }
                }
                else
                {
                    switch( s->argument_rgb1 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg1.xyz = color.xyz;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg1.xyz = colorVarying.xyz;" << std::endl;
                        break;
                    default:
                        ss << "  arg1.xyz = colorTex.xyz;" << std::endl;
                        break;
                    }

                    switch( s->argument_alpha1 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg1.w = color.w;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg1.w = colorVarying.w;" << std::endl;
                        break;
                    default:
                        ss << "  arg1.w = colorTex.w;" << std::endl;
                        break;
                    }
                }

                if( s->argument_rgb2 == s->argument_alpha2 && s->operation_rgb != MAGIC_TEXOP_ARGUMENT1 && s->operation_alpha != MAGIC_TEXOP_ARGUMENT1 )
                {
                    switch( s->argument_rgb2 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg2 = color;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg2 = colorVarying;" << std::endl;
                        break;
                    default:
                        ss << "  arg2 = colorTex;" << std::endl;
                        break;
                    }
                }
                else
                {
                    if( s->operation_rgb != MAGIC_TEXOP_ARGUMENT1 )
                    {
                        switch( s->argument_rgb2 )
                        {
                        case MAGIC_TEXARG_CURRENT:
                            ss << "  arg2.xyz = color.xyz;" << std::endl;
                            break;
                        case MAGIC_TEXARG_DIFFUSE:
                            ss << "  arg2.xyz = colorVarying.xyz;" << std::endl;
                            break;
                        default:
                            ss << "  arg2.xyz = colorTex.xyz;" << std::endl;
                            break;
                        }
                    }
                    if( s->operation_alpha != MAGIC_TEXOP_ARGUMENT1 )
                    {
                        switch( s->argument_alpha2 )
                        {
                        case MAGIC_TEXARG_CURRENT:
                            ss << "  arg2.w = color.w;" << std::endl;;
                            break;
                        case MAGIC_TEXARG_DIFFUSE:
                            ss << "  arg2.w = colorVarying.w;" << std::endl;;
                            break;
                        default:
                            ss << "  arg2.w = colorTex.w;" << std::endl;;
                            break;
                        }
                    }
                }

                if( s->operation_rgb == s->operation_alpha )
                {
                    switch( s->operation_rgb )
                    {
                    case MAGIC_TEXOP_ARGUMENT1:
                        ss << "  color = arg1;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_ADD:
                        ss << "  color = arg1 + arg2;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_SUBTRACT:
                        ss << "  color = arg1 - arg2;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE:
                        ss << "  color = arg1 * arg2;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE2X:
                        ss << "  color = arg1 * arg2;" << std::endl;;
                        ss << "  color = 2 * color;" << std::endl;;
                        break;
                    default:
                        ss << "  color = arg1 * arg2;" << std::endl;;
                        ss << "  color = 4 * color;" << std::endl;;
                        break;
                    }
                }
                else
                {
                    switch( s->operation_rgb )
                    {
                    case MAGIC_TEXOP_ARGUMENT1:
                        ss << "  color.xyz = arg1.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_ADD:
                        ss << "  color.xyz = arg1.xyz + arg2.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_SUBTRACT:
                        ss << "  color.xyz = arg1.xyz - arg2.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE:
                        ss << "  color.xyz = arg1.xyz * arg2.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE2X:
                        ss << "  color.xyz = arg1.xyz * arg2.xyz;" << std::endl;;
                        ss << "  color.xyz = 2 * color.xyz;" << std::endl;;
                        break;
                    default:
                        ss << "  color.xyz = arg1.xyz * arg2.xyz;" << std::endl;;
                        ss << "  color.xyz = 4 * color.xyz;" << std::endl;;
                        break;
                    }

                    switch( s->operation_alpha )
                    {
                    case MAGIC_TEXOP_ARGUMENT1:
                        ss << "  color.w = arg1.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_ADD:
                        ss << "  color.w = arg1.w + arg2.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_SUBTRACT:
                        ss << "  color.w = arg1.w - arg2.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE:
                        ss << "  color.w = arg1.w * arg2.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE2X:
                        ss << "  color.w = arg1.w * arg2.w;" << std::endl;;
                        ss << "  color.w = 2 * color.w;" << std::endl;;
                        break;
                    default:
                        ss << "  color.w = arg1.w * arg2.w;" << std::endl;;
                        ss << "  color.w = 4 * color.w;" << std::endl;;
                        break;
                    }
                }
            }

            ss << "  OUT.color = IN.color * color;" << std::endl;
        }
        else
        {
            ss << "  OUT.color = IN.color;" << std::endl;
        }

        ss << "}" << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxService::createFragmentShaderGLSource_( Stringstream & ss, const MAGIC_MATERIAL * m )
    {
        int32_t textures = m->textures;

        for( int32_t i = 0; i != textures; ++i )
        {
            ss << "uniform sampler2D inSampler" << i << ";" << std::endl;
        }

        ss << std::endl;

        ss << "varying vec4 v_Col;" << std::endl;

        for( int32_t i = 0; i != textures; ++i )
        {
            ss << "varying vec2 v_UV" << i << ";" << std::endl;
        }

        ss << std::endl;

        ss << "void main(void)" << std::endl;
        ss << "{" << std::endl;

        if( textures != 0 )
        {
            ss << "  vec4 color;" << std::endl;
            ss << "  vec4 arg1;" << std::endl;
            ss << "  vec4 arg2;" << std::endl;
            ss << "  vec4 colorTex;" << std::endl;
            ss << "  vec4 colorVarying=v_Col;" << std::endl;

            for( int32_t i = 0; i != textures; ++i )
            {
                ss << std::endl;

                MAGIC_TEXTURE_STATES * s = m->states + i;

                if( s->argument_rgb1 == MAGIC_TEXARG_TEXTURE ||
                    s->argument_alpha1 == MAGIC_TEXARG_TEXTURE ||
                    (s->operation_rgb != MAGIC_TEXOP_ARGUMENT1 && s->argument_rgb2 == MAGIC_TEXARG_TEXTURE) ||
                    (s->operation_alpha != MAGIC_TEXOP_ARGUMENT1 && s->argument_alpha2 == MAGIC_TEXARG_TEXTURE) )
                {
                    ss << "  colorTex = texture2D(inSampler" << i << ", v_UV" << i << ");" << std::endl;
                }

                if( s->argument_rgb1 == s->argument_alpha1 )
                {
                    switch( s->argument_rgb1 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg1 = color;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg1 = colorVarying;" << std::endl;
                        break;
                    default:
                        ss << "  arg1 = colorTex;" << std::endl;
                        break;
                    }
                }
                else
                {
                    switch( s->argument_rgb1 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg1.xyz = color.xyz;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg1.xyz = colorVarying.xyz;" << std::endl;
                        break;
                    default:
                        ss << "  arg1.xyz = colorTex.xyz;" << std::endl;
                        break;
                    }
                    switch( s->argument_alpha1 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg1.w = color.w;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg1.w = colorVarying.w;" << std::endl;
                        break;
                    default:
                        ss << "  arg1.w = colorTex.w;" << std::endl;
                        break;
                    }
                }

                if( s->argument_rgb2 == s->argument_alpha2 && s->operation_rgb != MAGIC_TEXOP_ARGUMENT1 && s->operation_alpha != MAGIC_TEXOP_ARGUMENT1 )
                {
                    switch( s->argument_rgb2 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg2 = color;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg2 = colorVarying;" << std::endl;
                        break;
                    default:
                        ss << "  arg2 = colorTex;" << std::endl;
                        break;
                    }
                }
                else
                {
                    if( s->operation_rgb != MAGIC_TEXOP_ARGUMENT1 )
                    {
                        switch( s->argument_rgb2 )
                        {
                        case MAGIC_TEXARG_CURRENT:
                            ss << "  arg2.xyz = color.xyz;" << std::endl;
                            break;
                        case MAGIC_TEXARG_DIFFUSE:
                            ss << "  arg2.xyz = colorVarying.xyz;" << std::endl;
                            break;
                        default:
                            ss << "  arg2.xyz = colorTex.xyz;" << std::endl;
                            break;
                        }
                    }
                    if( s->operation_alpha != MAGIC_TEXOP_ARGUMENT1 )
                    {
                        switch( s->argument_alpha2 )
                        {
                        case MAGIC_TEXARG_CURRENT:
                            ss << "  arg2.w = color.w;" << std::endl;;
                            break;
                        case MAGIC_TEXARG_DIFFUSE:
                            ss << "  arg2.w = colorVarying.w;" << std::endl;;
                            break;
                        default:
                            ss << "  arg2.w = colorTex.w;" << std::endl;;
                            break;
                        }
                    }
                }

                if( s->operation_rgb == s->operation_alpha )
                {
                    switch( s->operation_rgb )
                    {
                    case MAGIC_TEXOP_ARGUMENT1:
                        ss << "  color = arg1;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_ADD:
                        ss << "  color = arg1 + arg2;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_SUBTRACT:
                        ss << "  color = arg1 - arg2;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE:
                        ss << "  color = arg1 * arg2;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE2X:
                        ss << "  color = arg1 * arg2;" << std::endl;;
                        ss << "  color = 2 * color;" << std::endl;;
                        break;
                    default:
                        ss << "  color = arg1 * arg2;" << std::endl;;
                        ss << "  color = 4 * color;" << std::endl;;
                        break;
                    }
                }
                else
                {
                    switch( s->operation_rgb )
                    {
                    case MAGIC_TEXOP_ARGUMENT1:
                        ss << "  color.xyz = arg1.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_ADD:
                        ss << "  color.xyz = arg1.xyz + arg2.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_SUBTRACT:
                        ss << "  color.xyz = arg1.xyz - arg2.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE:
                        ss << "  color.xyz = arg1.xyz * arg2.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE2X:
                        ss << "  color.xyz = arg1.xyz * arg2.xyz;" << std::endl;;
                        ss << "  color.xyz = 2 * color.xyz;" << std::endl;;
                        break;
                    default:
                        ss << "  color.xyz = arg1.xyz * arg2.xyz;" << std::endl;;
                        ss << "  color.xyz = 4 * color.xyz;" << std::endl;;
                        break;
                    }

                    switch( s->operation_alpha )
                    {
                    case MAGIC_TEXOP_ARGUMENT1:
                        ss << "  color.w = arg1.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_ADD:
                        ss << "  color.w = arg1.w + arg2.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_SUBTRACT:
                        ss << "  color.w = arg1.w - arg2.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE:
                        ss << "  color.w = arg1.w * arg2.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE2X:
                        ss << "  color.w = arg1.w * arg2.w;" << std::endl;;
                        ss << "  color.w = 2 * color.w;" << std::endl;;
                        break;
                    default:
                        ss << "  color.w = arg1.w * arg2.w;" << std::endl;;
                        ss << "  color.w = 4 * color.w;" << std::endl;;
                        break;
                    }
                }
            }

            ss << "  gl_FragColor = v_Col * color;" << std::endl;
        }
        else
        {
            ss << "  gl_FragColor = v_Col;" << std::endl;
        }

        ss << "}" << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxService::createFragmentShaderGLESSource_( Stringstream & ss, const MAGIC_MATERIAL * m )
    {
        int32_t textures = m->textures;

        for( int32_t i = 0; i != textures; ++i )
        {
            ss << "uniform sampler2D inSampler" << i << ";" << std::endl;
        }

        ss << std::endl;

        ss << "varying vec4 v_Col;" << std::endl;

        for( int32_t i = 0; i != textures; ++i )
        {
            ss << "varying vec2 v_UV" << i << ";" << std::endl;
        }

        ss << std::endl;

        ss << "void main(void)" << std::endl;
        ss << "{" << std::endl;

        if( textures != 0 )
        {
            ss << "  mediump vec4 color;" << std::endl;
            ss << "  mediump vec4 arg1;" << std::endl;
            ss << "  mediump vec4 arg2;" << std::endl;
            ss << "  mediump vec4 colorTex;" << std::endl;
            ss << "  mediump vec4 colorVarying=v_Col;" << std::endl;

            for( int32_t i = 0; i != textures; ++i )
            {
                ss << std::endl;

                MAGIC_TEXTURE_STATES * s = m->states + i;

                if( s->argument_rgb1 == MAGIC_TEXARG_TEXTURE ||
                    s->argument_alpha1 == MAGIC_TEXARG_TEXTURE ||
                    (s->operation_rgb != MAGIC_TEXOP_ARGUMENT1 && s->argument_rgb2 == MAGIC_TEXARG_TEXTURE) ||
                    (s->operation_alpha != MAGIC_TEXOP_ARGUMENT1 && s->argument_alpha2 == MAGIC_TEXARG_TEXTURE) )
                {
                    ss << "  colorTex = texture2D(inSampler" << i << ", v_UV" << i << ");" << std::endl;
                }

                if( s->argument_rgb1 == s->argument_alpha1 )
                {
                    switch( s->argument_rgb1 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg1 = color;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg1 = colorVarying;" << std::endl;
                        break;
                    default:
                        ss << "  arg1 = colorTex;" << std::endl;
                        break;
                    }
                }
                else
                {
                    switch( s->argument_rgb1 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg1.xyz = color.xyz;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg1.xyz = colorVarying.xyz;" << std::endl;
                        break;
                    default:
                        ss << "  arg1.xyz = colorTex.xyz;" << std::endl;
                        break;
                    }
                    switch( s->argument_alpha1 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg1.w = color.w;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg1.w = colorVarying.w;" << std::endl;
                        break;
                    default:
                        ss << "  arg1.w = colorTex.w;" << std::endl;
                        break;
                    }
                }

                if( s->argument_rgb2 == s->argument_alpha2 && s->operation_rgb != MAGIC_TEXOP_ARGUMENT1 && s->operation_alpha != MAGIC_TEXOP_ARGUMENT1 )
                {
                    switch( s->argument_rgb2 )
                    {
                    case MAGIC_TEXARG_CURRENT:
                        ss << "  arg2 = color;" << std::endl;
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        ss << "  arg2 = colorVarying;" << std::endl;
                        break;
                    default:
                        ss << "  arg2 = colorTex;" << std::endl;
                        break;
                    }
                }
                else
                {
                    if( s->operation_rgb != MAGIC_TEXOP_ARGUMENT1 )
                    {
                        switch( s->argument_rgb2 )
                        {
                        case MAGIC_TEXARG_CURRENT:
                            ss << "  arg2.xyz = color.xyz;" << std::endl;
                            break;
                        case MAGIC_TEXARG_DIFFUSE:
                            ss << "  arg2.xyz = colorVarying.xyz;" << std::endl;
                            break;
                        default:
                            ss << "  arg2.xyz = colorTex.xyz;" << std::endl;
                            break;
                        }
                    }
                    if( s->operation_alpha != MAGIC_TEXOP_ARGUMENT1 )
                    {
                        switch( s->argument_alpha2 )
                        {
                        case MAGIC_TEXARG_CURRENT:
                            ss << "  arg2.w = color.w;" << std::endl;;
                            break;
                        case MAGIC_TEXARG_DIFFUSE:
                            ss << "  arg2.w = colorVarying.w;" << std::endl;;
                            break;
                        default:
                            ss << "  arg2.w = colorTex.w;" << std::endl;;
                            break;
                        }
                    }
                }

                if( s->operation_rgb == s->operation_alpha )
                {
                    switch( s->operation_rgb )
                    {
                    case MAGIC_TEXOP_ARGUMENT1:
                        ss << "  color = arg1;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_ADD:
                        ss << "  color = arg1 + arg2;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_SUBTRACT:
                        ss << "  color = arg1 - arg2;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE:
                        ss << "  color = arg1 * arg2;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE2X:
                        ss << "  color = arg1 * arg2;" << std::endl;;
                        ss << "  color = 2 * color;" << std::endl;;
                        break;
                    default:
                        ss << "  color = arg1 * arg2;" << std::endl;;
                        ss << "  color = 4 * color;" << std::endl;;
                        break;
                    }
                }
                else
                {
                    switch( s->operation_rgb )
                    {
                    case MAGIC_TEXOP_ARGUMENT1:
                        ss << "  color.xyz = arg1.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_ADD:
                        ss << "  color.xyz = arg1.xyz + arg2.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_SUBTRACT:
                        ss << "  color.xyz = arg1.xyz - arg2.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE:
                        ss << "  color.xyz = arg1.xyz * arg2.xyz;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE2X:
                        ss << "  color.xyz = arg1.xyz * arg2.xyz;" << std::endl;;
                        ss << "  color.xyz = 2 * color.xyz;" << std::endl;;
                        break;
                    default:
                        ss << "  color.xyz = arg1.xyz * arg2.xyz;" << std::endl;;
                        ss << "  color.xyz = 4 * color.xyz;" << std::endl;;
                        break;
                    }

                    switch( s->operation_alpha )
                    {
                    case MAGIC_TEXOP_ARGUMENT1:
                        ss << "  color.w = arg1.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_ADD:
                        ss << "  color.w = arg1.w + arg2.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_SUBTRACT:
                        ss << "  color.w = arg1.w - arg2.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE:
                        ss << "  color.w = arg1.w * arg2.w;" << std::endl;;
                        break;
                    case MAGIC_TEXOP_MODULATE2X:
                        ss << "  color.w = arg1.w * arg2.w;" << std::endl;;
                        ss << "  color.w = 2 * color.w;" << std::endl;;
                        break;
                    default:
                        ss << "  color.w = arg1.w * arg2.w;" << std::endl;;
                        ss << "  color.w = 4 * color.w;" << std::endl;;
                        break;
                    }
                }
            }

            ss << "  gl_FragColor = v_Col * color;" << std::endl;
        }
        else
        {
            ss << "  gl_FragColor = v_Col;" << std::endl;
        }

        ss << "}" << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr AstralaxService::cacheFragmentShader_( const MAGIC_MATERIAL * m )
    {
        int32_t textures = m->textures;

        for( const MagicStatesCache & cache : m_renderFragmentShaderCache )
        {
            if( cache.textures != textures )
            {
                continue;
            }

            for( int32_t i = 0; i != textures; ++i )
            {
                const MAGIC_TEXTURE_STATES * a0 = cache.states + i;
                const MAGIC_TEXTURE_STATES * a1 = m->states + i;

                if( a0->address_u != a1->address_u )
                {
                    continue;
                }

                if( a0->address_v != a1->address_v )
                {
                    continue;
                }

                if( a0->operation_rgb != a1->operation_rgb )
                {
                    continue;
                }

                if( a0->argument_rgb1 != a1->argument_rgb1 )
                {
                    continue;
                }

                if( a0->argument_rgb2 != a1->argument_rgb2 )
                {
                    continue;
                }

                if( a0->operation_alpha != a1->operation_alpha )
                {
                    continue;
                }

                if( a0->argument_alpha1 != a1->argument_alpha1 )
                {
                    continue;
                }

                if( a0->argument_alpha2 != a1->argument_alpha2 )
                {
                    continue;
                }
            }

            return cache.fragmentShader;
        }

#ifdef MENGINE_DEBUG
        Stringstream ss;
        switch( m_renderPlatform )
        {
        case RP_DX9:
            {
                this->createFragmentShaderDX9Source_( ss, m );
            }break;
        case RP_DX11:
            {
                this->createFragmentShaderDX11Source_( ss, m );
            }break;
        case RP_OPENGL:
            {
                this->createFragmentShaderGLSource_( ss, m );
            }break;
        case RP_OPENGLES:
            {
                this->createFragmentShaderGLESSource_( ss, m );
            }break;
        default:
            {
                LOGGER_ERROR( "not supported render platform [%u]"
                    , m_renderPlatform
                );

                return nullptr;
            }break;
        }
#endif

        RenderFragmentShaderInterfacePtr fragmentShader = nullptr;

        switch( textures )
        {
        case 0:
            {
                fragmentShader = RENDERMATERIAL_SERVICE()
                    ->getFragmentShader( STRINGIZE_STRING_LOCAL( "Fragment_Color" ) );
            }break;
        case 1:
            {
                MAGIC_TEXTURE_STATES * state = m->states + 0;

                if( state->operation_rgb == MAGIC_TEXOP_MODULATE &&
                    state->argument_rgb1 == MAGIC_TEXARG_DIFFUSE &&
                    state->argument_rgb2 == MAGIC_TEXARG_TEXTURE &&
                    state->operation_alpha == MAGIC_TEXOP_MODULATE &&
                    state->argument_alpha1 == MAGIC_TEXARG_DIFFUSE &&
                    state->argument_alpha2 == MAGIC_TEXARG_TEXTURE )
                {
                    fragmentShader = RENDERMATERIAL_SERVICE()
                        ->getFragmentShader( STRINGIZE_STRING_LOCAL( "Fragment_Blend" ) );
                }
            }break;
        default:
            {

            }break;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( fragmentShader, "not support this astralax particle shader" );

        MagicStatesCache key;
        key.textures = textures;

        MENGINE_MEMSET( key.states + 0, 0, sizeof( MAGIC_TEXTURE_STATES ) );
        MENGINE_MEMSET( key.states + 1, 0, sizeof( MAGIC_TEXTURE_STATES ) );

        for( int32_t i = 0; i != textures; ++i )
        {
            key.states[i] = m->states[i];
        }

        key.fragmentShader = fragmentShader;

        m_renderFragmentShaderCache.emplace_back( key );

        return fragmentShader;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxService::onEmitterContainerRelease_( AstralaxEmitterContainer * _container )
    {
        uint32_t id = _container->getPtcId();

        if( id == 0 )
        {
            return;
        }

        MapHashEmitterContainers::iterator it_found = m_containers.find( id );

        MENGINE_ASSERTION( it_found != m_containers.end(), "deleted container not found on cache (doc: %s)"
            , MENGINE_DOCUMENT_STR( _container->getDocument() )
        );

        AstralaxEmitterContainerDesc & desc = it_found->second;
        --desc.reference;

        if( desc.reference == 0 )
        {
            m_containers.erase( it_found );

            _container->finalize();

            this->updateAtlas();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxService::onEmitterRelease_( AstralaxEmitter2 * _emitter )
    {
        _emitter->finalize();

        this->updateAtlas();
    }
    //////////////////////////////////////////////////////////////////////////
}
