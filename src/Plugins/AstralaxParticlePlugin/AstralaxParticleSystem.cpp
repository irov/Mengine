#include "AstralaxParticleSystem.h"

#include "Interface/StringizeServiceInterface.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Kernel/String.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AstralaxSystem, Mengine::AstralaxParticleSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AstralaxParticleSystem::AstralaxParticleSystem()
        : m_renderPlatform( RP_UNKNOWN )
        , m_materialCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxParticleSystem::~AstralaxParticleSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxParticleSystem::_initializeService()
    {
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

        if( strstr( version, MAGIC_API ) == nullptr )
        {
            LOGGER_CRITICAL( "Astralax Magic Particles Version: '%s' but need '%s'"
                , version
                , MAGIC_API
            );

            return false;
        }

        LOGGER_MESSAGE( "Astralax Magic Particles Version: %s"
            , version
        );

        m_renderPlatform = RENDER_SYSTEM()
            ->getRenderPlatformType();

        m_factoryPoolAstralaxEmitterContainer = Helper::makeFactoryPoolWithListener<AstralaxEmitterContainer, 16>( this, &AstralaxParticleSystem::onEmitterContainerRelease_ );
        m_factoryPoolAstralaxEmitter = Helper::makeFactoryPoolWithListener<AstralaxEmitter2, 16>( this, &AstralaxParticleSystem::onEmitterRelease_ );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticleSystem::_finalizeService()
    {
        m_atlases.clear();
        m_containers.clear();

        m_renderFragmentShaderCache.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPoolAstralaxEmitterContainer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPoolAstralaxEmitter );

        m_factoryPoolAstralaxEmitterContainer = nullptr;
        m_factoryPoolAstralaxEmitter = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitterContainerInterfacePtr AstralaxParticleSystem::createEmitterContainerFromMemory( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, const ArchivatorInterfacePtr & _archivator, const Char * _doc )
    {
        AstralaxEmitterContainer2Ptr container = m_factoryPoolAstralaxEmitterContainer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( container, nullptr, "invalid create container doc '%s'"
            , _doc
        );

        if( container->initialize( _fileGroup, _fileName, _archivator ) == false )
        {
            LOGGER_ERROR( "invalid initialize container doc '%s'"
                , _doc
            );

            return nullptr;
        }

        uint32_t id = container->getPtcId();

        MapHashEmitterContainers::iterator it_found = m_containers.find( id );

        if( it_found == m_containers.end() )
        {
            AstralaxEmitterContainerDesc new_desc;
            new_desc.reference = 0;
            new_desc.container = container.get();
#ifdef MENGINE_DEBUG
            new_desc.doc = _doc;
#endif

            it_found = m_containers.emplace( id, new_desc ).first;
        }
        else
        {
            container->finalize();

#ifdef MENGINE_DEBUG
            LOGGER_PERFORMANCE( "useless load container '%s' original is '%s'"
                , _doc
                , it_found->second.doc.c_str()
            );
#endif
        }

        AstralaxEmitterContainerDesc & desc = it_found->second;
        ++desc.reference;

        AstralaxEmitterContainer2Ptr new_container( desc.container );

        return new_container;
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitterInterfacePtr AstralaxParticleSystem::createEmitter( const AstralaxEmitterContainerInterfacePtr & _container, const Char * _doc )
    {
        AstralaxEmitter2Ptr emitter = m_factoryPoolAstralaxEmitter->createObject( _doc );

        if( emitter->initialize( this, _container ) == false )
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
    bool AstralaxParticleSystem::updateAtlas()
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

                    m_atlases.emplace_back( resourceImage );
                }break;
            case MAGIC_CHANGE_ATLAS_DELETE:
                {
                    VectorAtlasDesc::iterator it_remove = m_atlases.begin();
                    std::advance( it_remove, c.index );
                    m_atlases.erase( it_remove );
                }break;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxParticleSystem::updateMaterial()
    {
        int32_t newMaterialCount = Magic_GetMaterialCount();

        for( int32_t i = m_materialCount; i != newMaterialCount; ++i )
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

            MENGINE_ASSERTION_MEMORY_PANIC( vertexAttribute, false );
            MENGINE_ASSERTION_MEMORY_PANIC( vertexShader, false );

            RenderFragmentShaderInterfacePtr fragmentShader = this->cacheFragmentShader_( &m );

            MENGINE_ASSERTION_MEMORY_PANIC( fragmentShader, false );

            RenderProgramInterfacePtr program = RENDER_SYSTEM()
                ->createProgram( STRINGIZE_STRING_LOCAL( "AstralaxProgram" ), vertexShader, fragmentShader, vertexAttribute, m.textures, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( program, false );

            rs.program = program;

            for( int32_t stage = 0; stage != m.textures; ++stage )
            {
                const MAGIC_TEXTURE_STATES & state = m.states[stage];

                const ETextureAddressMode dx_address[] = { TAM_WRAP, TAM_MIRROR, TAM_CLAMP, TAM_BORDER };

                RenderTextureStage & textureStage = rs.textureStage[stage];

                textureStage.addressU = dx_address[state.address_u];
                textureStage.addressV = dx_address[state.address_v];
                textureStage.addressBorder = 0x00000000;

                //const ETextureOp dx_operation[] = {TOP_SELECTARG1, TOP_ADD, TOP_SUBTRACT, TOP_MODULATE, TOP_MODULATE2X, TOP_MODULATE4X};
                //const ETextureArgument dx_arg[] = {TARG_CURRENT, TARG_DIFFUSE, TARG_TEXTURE};

    //            textureStage.colorOp = dx_operation[state.operation_rgb];
    //            textureStage.colorArg1 = dx_arg[state.argument_rgb1];
    //            textureStage.colorArg2 = dx_arg[state.argument_rgb2];

    //            textureStage.alphaOp = dx_operation[state.operation_alpha];
    //            textureStage.alphaArg1 = dx_arg[state.argument_alpha1];
    //            textureStage.alphaArg2 = dx_arg[state.argument_alpha2];
            }



            const RenderMaterialStage * cache_stage = RENDERMATERIAL_SERVICE()
                ->cacheStage( rs );

            m_stages[i] = cache_stage;
        }

        m_materialCount = newMaterialCount;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialStage * AstralaxParticleSystem::getMaterialStage( int32_t _index ) const
    {
        if( _index >= m_materialCount )
        {
            return nullptr;
        }

        return m_stages[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & AstralaxParticleSystem::getResourceImage( int32_t _index ) const
    {
        VectorAtlasDesc::size_type atlases_size = m_atlases.size();

        MENGINE_ASSERTION_RETURN( (VectorAtlasDesc::size_type)_index < atlases_size, ResourceImagePtr::none(), "index %d but size is %d"
            , _index
            , atlases_size
        );

        const ResourceImagePtr & resourceImage = m_atlases[_index];

        return resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AstralaxParticleSystem::getEmitterCount() const
    {
        uint32_t countObject = m_factoryPoolAstralaxEmitter->getCountObject();

        return countObject;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticleSystem::createFragmentShaderDX9Source_( Stringstream & ss, const MAGIC_MATERIAL * m )
    {
        MAGIC_VERTEX_FORMAT vertex_format = m->format;

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

                MAGIC_TEXTURE_STATES* s = m->states + i;

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
    void AstralaxParticleSystem::createFragmentShaderGLSource_( Stringstream & ss, const MAGIC_MATERIAL * m )
    {
        MAGIC_VERTEX_FORMAT vertex_format = m->format;

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

                MAGIC_TEXTURE_STATES* s = m->states + i;

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
    RenderFragmentShaderInterfacePtr AstralaxParticleSystem::cacheFragmentShader_( const MAGIC_MATERIAL * m )
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
        case RP_OPENGL:
            {
                this->createFragmentShaderGLSource_( ss, m );
            }break;
        default:
            {
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

        MENGINE_ASSERTION_MEMORY_PANIC( fragmentShader, nullptr, "not support this particle shader" );

        MagicStatesCache key;
        key.textures = textures;

        memset( key.states + 0, 0, sizeof( MAGIC_TEXTURE_STATES ) );
        memset( key.states + 1, 0, sizeof( MAGIC_TEXTURE_STATES ) );

        for( int32_t i = 0; i != textures; ++i )
        {
            key.states[i] = m->states[i];
        }

        key.fragmentShader = fragmentShader;

        m_renderFragmentShaderCache.emplace_back( key );

        return fragmentShader;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticleSystem::onEmitterContainerRelease_( AstralaxEmitterContainer * _container )
    {
        uint32_t id = _container->getPtcId();

        if( id == 0 )
        {
            return;
        }

        MapHashEmitterContainers::iterator it_found = m_containers.find( id );

        MENGINE_ASSERTION( it_found != m_containers.end(), "deleted container not found on cache" );

        AstralaxEmitterContainerDesc & desc = it_found->second;
        --desc.reference;

        if( desc.reference == 0 )
        {
            m_containers.erase( it_found );

            _container->finalize();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticleSystem::onEmitterRelease_( AstralaxEmitter2 * _emitter )
    {
        _emitter->finalize();
    }
}