#include "RenderMaterialService.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"

#include "Kernel/MemoryHelper.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "Metacode/Metacode.h"

#include "stdex/hash.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderMaterialService, Mengine::RenderMaterialService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static ETextureFilter parseConfigTextureFilterValue( uint32_t _value )
    {
        switch( _value )
        {
        case 0:
            return TF_NONE;
            break;
        case 1:
            return TF_POINT;
            break;
        case 2:
            return TF_LINEAR;
            break;
        case 3:
            return TF_ANISOTROPIC;
            break;
        case 4:
            return TF_FLATCUBIC;
            break;
        case 5:
            return TF_GAUSSIANCUBIC;
            break;
        default:
            break;
        }

        return TF_GAUSSIANCUBIC;
    }
    //////////////////////////////////////////////////////////////////////////    
    RenderMaterialService::RenderMaterialService()
        : m_materialEnumerator( 0 )
        , m_stageCount( 0 )
        , m_defaultTextureFilterMipmap( TF_NONE )
        , m_defaultTextureFilterMagnification( TF_LINEAR )
        , m_defaultTextureFilterMinification( TF_LINEAR )
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    RenderMaterialService::~RenderMaterialService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderMaterialService::_initializeService()
    {
        m_defaultStageNames[EM_DEBUG] = STRINGIZE_STRING_LOCAL( "Debug" );

        m_defaultStageNames[EM_TEXTURE_SOLID] = STRINGIZE_STRING_LOCAL( "Texture_Solid" );

        m_defaultStageNames[EM_TEXTURE_BLEND] = STRINGIZE_STRING_LOCAL( "Texture_Blend" );
        m_defaultStageNames[EM_TEXTURE_BLEND_WC] = STRINGIZE_STRING_LOCAL( "Texture_Blend_WC" );
        m_defaultStageNames[EM_TEXTURE_BLEND_WW] = STRINGIZE_STRING_LOCAL( "Texture_Blend_WW" );
        m_defaultStageNames[EM_TEXTURE_BLEND_CW] = STRINGIZE_STRING_LOCAL( "Texture_Blend_CW" );
        m_defaultStageNames[EM_TEXTURE_INTENSIVE] = STRINGIZE_STRING_LOCAL( "Texture_Intensive" );
        m_defaultStageNames[EM_TEXTURE_MULTIPLY] = STRINGIZE_STRING_LOCAL( "Texture_Multiply" );
        m_defaultStageNames[EM_TEXTURE_MULTIPLY_PREMULTIPLY] = STRINGIZE_STRING_LOCAL( "Texture_Multiply_Premultiply" );
        m_defaultStageNames[EM_TEXTURE_SCREEN] = STRINGIZE_STRING_LOCAL( "Texture_Screen" );
        m_defaultStageNames[EM_TEXTURE_SCREEN_PREMULTIPLY] = STRINGIZE_STRING_LOCAL( "Texture_Screen_Premultiply" );

        m_defaultStageNames[EM_TEXTURE_BLEND_PREMULTIPLY] = STRINGIZE_STRING_LOCAL( "Texture_Blend_Premultiply" );
        m_defaultStageNames[EM_TEXTURE_INTENSIVE_PREMULTIPLY] = STRINGIZE_STRING_LOCAL( "Texture_Intensive_Premultiply" );

        m_defaultStageNames[EM_TEXTURE_BLEND_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( "Texture_Blend_OnlyColor" );
        m_defaultStageNames[EM_TEXTURE_INTENSIVE_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( "Texture_Intensive_OnlyColor" );
        m_defaultStageNames[EM_TEXTURE_MULTIPLY_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( "Texture_Multiply_OnlyColor" );
        m_defaultStageNames[EM_TEXTURE_SCREEN_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( "Texture_Screen_OnlyColor" );

        m_defaultStageNames[EM_TEXTURE_BLEND_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( "Texture_Blend_ExternalAlpha" );
        m_defaultStageNames[EM_TEXTURE_BLEND_EXTERNAL_ALPHA_WC] = STRINGIZE_STRING_LOCAL( "Texture_Blend_ExternalAlpha_WC" );
        m_defaultStageNames[EM_TEXTURE_BLEND_EXTERNAL_ALPHA_WW] = STRINGIZE_STRING_LOCAL( "Texture_Blend_ExternalAlpha_WW" );
        m_defaultStageNames[EM_TEXTURE_BLEND_EXTERNAL_ALPHA_CW] = STRINGIZE_STRING_LOCAL( "Texture_Blend_ExternalAlpha_CW" );
        m_defaultStageNames[EM_TEXTURE_INTENSIVE_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( "Texture_Intensive_ExternalAlpha" );
        m_defaultStageNames[EM_TEXTURE_MULTIPLY_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( "Texture_Multiply_ExternalAlpha" );
        m_defaultStageNames[EM_TEXTURE_SCREEN_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( "Texture_Screen_ExternalAlpha" );

        m_defaultStageNames[EM_TEXTURE_BLEND_EXTERNAL_ALPHA_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( "Texture_Blend_ExternalAlpha_OnlyColor" );
        m_defaultStageNames[EM_TEXTURE_INTENSIVE_EXTERNAL_ALPHA_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( "Texture_Intensive_ExternalAlpha_OnlyColor" );
        m_defaultStageNames[EM_TEXTURE_MULTIPLY_EXTERNAL_ALPHA_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( "Texture_Multiply_ExternalAlpha_OnlyColor" );
        m_defaultStageNames[EM_TEXTURE_SCREEN_EXTERNAL_ALPHA_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( "Texture_Screen_ExternalAlpha_OnlyColor" );

        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_BLEND] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatte_Blend" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_BLEND_PREMULTIPLY] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatte_Blend_Premultiply" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_INTENSIVE] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatte_Intensive" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_INTENSIVE_PREMULTIPLY] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatte_Intensive_Premultiply" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_INVERTED_BLEND] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatteInverted_Blend" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_INVERTED_BLEND_PREMULTIPLY] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatteInverted_Blend_Premultiply" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatteInverted_Intensive" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE_PREMULTIPLY] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatteInverted_Intensive_Premultiply" );

        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_BLEND_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatte_Blend_ExternalAlpha" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_BLEND_PREMULTIPLY_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatte_Blend_Premultiply_ExternalAlpha" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_INTENSIVE_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatte_Intensive_ExternalAlpha" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_INTENSIVE_PREMULTIPLY_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatte_Intensive_Premultiply_ExternalAlpha" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_INVERTED_BLEND_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatteInverted_Blend_ExternalAlpha" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_INVERTED_BLEND_PREMULTIPLY_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatteInverted_Blend_Premultiply_ExternalAlpha" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatteInverted_Intensive_ExternalAlpha" );
        m_defaultStageNames[EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE_PREMULTIPLY_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( "Texture_TrackMatteInverted_Intensive_Premultiply_ExternalAlpha" );

        m_defaultStageNames[EM_TEXTURE_ALPHAMASK_BLEND] = STRINGIZE_STRING_LOCAL( "Texture_AlphaMask_Blend" );

        m_defaultStageNames[EM_COLOR_SOLID] = STRINGIZE_STRING_LOCAL( "Color_Solid" );
        m_defaultStageNames[EM_COLOR_BLEND] = STRINGIZE_STRING_LOCAL( "Color_Blend" );
        m_defaultStageNames[EM_COLOR_INTENSIVE] = STRINGIZE_STRING_LOCAL( "Color_Intensive" );
        m_defaultStageNames[EM_COLOR_MULTIPLY] = STRINGIZE_STRING_LOCAL( "Color_Multiply" );
        m_defaultStageNames[EM_COLOR_SCREEN] = STRINGIZE_STRING_LOCAL( "Color_Screen" );

        for( uint32_t i = 0; i != EM_MATERIAL_COUNT; ++i )
        {
            const ConstString & name = m_defaultStageNames[i];

            m_defaultStagesEnum[name] = (EMaterial)i;
        }

        for( uint32_t i = 0; i != EM_MATERIAL_COUNT; ++i )
        {
            m_defaultStages[i] = nullptr;
        }

        uint32_t defaultTextureFilterMipmap = CONFIG_VALUE( "Engine", "DefaultTextureFilterMipmap", 0U );
        uint32_t defaultTextureFilterMagnification = CONFIG_VALUE( "Engine", "DefaultTextureFilterMagnification", 2U );
        uint32_t defaultTextureFilterMinification = CONFIG_VALUE( "Engine", "DefaultTextureFilterMinification", 2U );

        if( defaultTextureFilterMipmap > 2 )
        {
            LOGGER_ERROR( "DefaultTextureFilterMipmap undefined great 2 current '%u'"
                , defaultTextureFilterMipmap
            );

            return false;
        }

        if( defaultTextureFilterMagnification > 2 )
        {
            LOGGER_ERROR( "DefaultTextureFilterMagnification undefined great 2 current '%u'"
                , defaultTextureFilterMagnification
            );

            return false;
        }

        m_defaultTextureFilterMipmap = parseConfigTextureFilterValue( defaultTextureFilterMipmap );
        m_defaultTextureFilterMagnification = parseConfigTextureFilterValue( defaultTextureFilterMagnification );
        m_defaultTextureFilterMinification = parseConfigTextureFilterValue( defaultTextureFilterMinification );

        m_factoryMaterial = Helper::makeFactoryPoolWithListener<RenderMaterial, 256>( this, &RenderMaterialService::onRenderMaterialDestroy_ );

        m_materialEnumerators.reserve( 1024 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderMaterialService::_finalizeService()
    {
        for( uint32_t i = 0; i != MENGINE_RENDER_MATERIAL_HASH_TABLE_SIZE; ++i )
        {
            IntrusiveListRenderMaterial & material = m_materials[i];

            material.clear();
        }

        for( uint32_t i = 0; i != MENGINE_MATERIAL_RENDER_STAGE_MAX; ++i )
        {
            RenderMaterialStage & stage = m_stages[i];

            stage.program = nullptr;
        }

        m_materialStageIndexer.clear();

        m_vertexAttributes.clear();
        m_vertexShaders.clear();
        m_fragmentShaders.clear();
        m_programs.clear();

        m_debugLineMaterial = nullptr;
        m_debugTriangleMaterial = nullptr;

        m_solidRenderMaterial[0] = nullptr;
        m_solidRenderMaterial[1] = nullptr;
        m_solidRenderMaterial[2] = nullptr;
        m_solidRenderMaterial[3] = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMaterial );

        m_factoryMaterial = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderMaterialService::loadMaterials( const FileGroupInterfacePtr& _fileGroup, const FilePath& _fileName )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _fileGroup, _fileName, &datablock, Metacode::Meta_Data::getVersion(), exist ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "materials '%s:%s' not found"
                    , _fileGroup->getName().c_str()
                    , _fileName.c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse materials '%s:%s'"
                    , _fileGroup->getName().c_str()
                    , _fileName.c_str()
                );
            }

            return false;
        }

        const ConstString & renderPlatformName = RENDER_SYSTEM()
            ->getRenderPlatformName();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_FragmentShader & includes_FragmentShader = datablock.get_Includes_FragmentShader();

        for( Metacode::Meta_Data::Meta_DataBlock::VectorMeta_FragmentShader::const_iterator
            it = includes_FragmentShader.begin(),
            it_end = includes_FragmentShader.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_FragmentShader & meta_FragmentShader = *it;

            const ConstString & name = meta_FragmentShader.get_Name();
            const ConstString & renderPlatform = meta_FragmentShader.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            const FilePath & filePath = meta_FragmentShader.get_File_Path();

            bool isCompile = false;
            meta_FragmentShader.get_File_Compile( &isCompile );

            RenderFragmentShaderInterfacePtr shader = this->createFragmentShader_( name, _fileGroup, filePath, isCompile );

            if( shader == nullptr )
            {
                LOGGER_ERROR( "material %s:%s invalid load fragment shader %s compile %d"
                    , _fileGroup->getName().c_str()
                    , _fileName.c_str()
                    , filePath.c_str()
                    , isCompile
                );

                return false;
            }

            m_fragmentShaders.emplace( name, shader );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_VertexShader & includes_VertexShader = datablock.get_Includes_VertexShader();

        for( Metacode::Meta_Data::Meta_DataBlock::VectorMeta_VertexShader::const_iterator
            it = includes_VertexShader.begin(),
            it_end = includes_VertexShader.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexShader & meta_VertexShader = *it;

            const ConstString & renderPlatform = meta_VertexShader.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            const ConstString & name = meta_VertexShader.get_Name();

            const FilePath & filePath = meta_VertexShader.get_File_Path();

            bool isCompile = false;
            meta_VertexShader.get_File_Compile( &isCompile );

            RenderVertexShaderInterfacePtr shader = this->createVertexShader_( name, _fileGroup, filePath, isCompile );

            if( shader == nullptr )
            {
                LOGGER_ERROR( "material %s:%s invalid load vertex shader '%s' compile %d"
                    , _fileGroup->getName().c_str()
                    , _fileName.c_str()
                    , filePath.c_str()
                    , isCompile
                );

                return false;
            }

            m_vertexShaders.emplace( name, shader );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_VertexAttribute & includes_VertexAttribute = datablock.get_Includes_VertexAttribute();

        for( Metacode::Meta_Data::Meta_DataBlock::VectorMeta_VertexAttribute::const_iterator
            it = includes_VertexAttribute.begin(),
            it_end = includes_VertexAttribute.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexAttribute & meta_VertexAttribute = *it;

            const ConstString & renderPlatform = meta_VertexAttribute.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            uint32_t elementSize = meta_VertexAttribute.get_Element_Size();

            const ConstString & name = meta_VertexAttribute.get_Name();

            RenderVertexAttributeInterfacePtr vertexAttribute = this->createVertexAttribute_( name, elementSize );

            const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexAttribute::VectorMeta_Attribute & includes_Attributes = meta_VertexAttribute.get_Includes_Attribute();

            for( const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute & meta_Attribute : includes_Attributes )
            {
                const ConstString & attribute_uniform = meta_Attribute.get_Uniform();
                uint32_t attribute_size = meta_Attribute.get_Size();
                EVertexAttributeType attribute_type = meta_Attribute.get_Type();
                bool attribute_normalized = meta_Attribute.get_Normalized();
                uint32_t attribute_stride = meta_Attribute.get_Stride();
                uint32_t attribute_offset = meta_Attribute.get_Offset();

                vertexAttribute->addAttribute( attribute_uniform, attribute_size, attribute_type, attribute_normalized, attribute_stride, attribute_offset );
            }

            m_vertexAttributes.emplace( name, vertexAttribute );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Program & includes_Program = datablock.get_Includes_Program();

        for( Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Program::const_iterator
            it = includes_Program.begin(),
            it_end = includes_Program.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_Program & meta_Program = *it;

            ConstString renderPlatform;
            if( meta_Program.get_RenderPlatform( &renderPlatform ) == true )
            {
                if( renderPlatform != renderPlatformName )
                {
                    continue;
                }
            }

            const ConstString & name = meta_Program.get_Name();

            const ConstString & vertexShaderName = meta_Program.get_VertexShader_Name();
            const ConstString & fragmentShaderName = meta_Program.get_FragmentShader_Name();
            const ConstString & vertexAttributeName = meta_Program.get_VertexAttribute_Name();
            uint32_t samplerCount = meta_Program.get_Sampler_Count();

            const RenderVertexShaderInterfacePtr & vertexShader = this->getVertexShader( vertexShaderName );

            if( vertexShader == nullptr )
            {
                LOGGER_ERROR( "material %s:%s program '%s' not found vertex shader '%s'"
                    , _fileGroup->getName().c_str()
                    , _fileName.c_str()
                    , name.c_str()
                    , vertexShaderName.c_str()
                );

                return false;
            }

            const RenderFragmentShaderInterfacePtr & fragmentShader = this->getFragmentShader( fragmentShaderName );

            if( fragmentShader == nullptr )
            {
                LOGGER_ERROR( "material '%s:%s' program '%s' not found fragment shader '%s'"
                    , _fileGroup->getName().c_str()
                    , _fileName.c_str()
                    , name.c_str()
                    , fragmentShaderName.c_str()
                );

                return false;
            }

            const RenderVertexAttributeInterfacePtr & vertexAttribute = this->getVertexAttribute( vertexAttributeName );

            if( vertexAttribute == nullptr )
            {
                LOGGER_ERROR( "material '%s:%s' program '%s' not found vertex attribute '%s'"
                    , _fileGroup->getName().c_str()
                    , _fileName.c_str()
                    , name.c_str()
                    , vertexAttributeName.c_str()
                );

                return false;
            }

            RenderProgramInterfacePtr program = RENDER_SYSTEM()
                ->createProgram( name, vertexShader, fragmentShader, vertexAttribute, samplerCount, MENGINE_DOCUMENT_FUNCTION );

            if( program == nullptr )
            {
                LOGGER_ERROR( "material '%s:%s' invalid create program vertex '%s' fragment '%s'"
                    , _fileGroup->getName().c_str()
                    , _fileName.c_str()
                    , vertexShaderName.c_str()
                    , fragmentShaderName.c_str()
                );

                return false;
            }

            m_programs.emplace( name, program );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Material & includes_Material = datablock.get_Includes_Material();

        for( Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Material::const_iterator
            it = includes_Material.begin(),
            it_end = includes_Material.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_Material & meta_Material = *it;

            const ConstString & name = meta_Material.get_Name();

            ConstString renderPlatform;
            if( meta_Material.get_RenderPlatform( &renderPlatform ) == true )
            {
                if( renderPlatform != renderPlatformName )
                {
                    continue;
                }
            }

            bool is_debug = false;
            meta_Material.get_Debug( &is_debug );

            RenderMaterialStage stage;
            meta_Material.get_AlphaBlend_Enable( &stage.alphaBlendEnable );
            meta_Material.get_BlendFactor_Source( &stage.blendSrc );
            meta_Material.get_BlendFactor_Dest( &stage.blendDst );
            meta_Material.get_BlendFactor_Op( &stage.blendOp );

            ConstString programName;
            if( meta_Material.get_Program_Name( &programName ) == true )
            {
                const RenderProgramInterfacePtr & program = this->getProgram( programName );

                if( program == nullptr )
                {
                    LOGGER_ERROR( "material '%s:%s' invalid get program '%s'"
                        , _fileGroup->getName().c_str()
                        , _fileName.c_str()
                        , programName.c_str()
                    );

                    return false;
                }

                stage.program = program;
            }

            const Metacode::Meta_Data::Meta_DataBlock::Meta_Material::VectorMeta_TextureStages & include_TextureStages = meta_Material.get_Includes_TextureStages();

            for( Metacode::Meta_Data::Meta_DataBlock::Meta_Material::VectorMeta_TextureStages::const_iterator
                it_include = include_TextureStages.begin(),
                it_include_end = include_TextureStages.end();
                it_include != it_include_end;
                ++it_include )
            {
                const Metacode::Meta_Data::Meta_DataBlock::Meta_Material::Meta_TextureStages & meta_TextureStages = *it_include;

                uint32_t index = meta_TextureStages.get_Stage();

                RenderTextureStage & textureStage = stage.textureStage[index];

                textureStage.mipmap = m_defaultTextureFilterMipmap;
                textureStage.magnification = m_defaultTextureFilterMagnification;
                textureStage.minification = m_defaultTextureFilterMinification;

                meta_TextureStages.get_AddressMode_U( &textureStage.addressU );
                meta_TextureStages.get_AddressMode_V( &textureStage.addressV );
                meta_TextureStages.get_AddressMode_Border( &textureStage.addressBorder );
            }

            const RenderMaterialStage * cache_stage = this->createRenderStageGroup( name, stage );

            if( cache_stage == nullptr )
            {
                LOGGER_ERROR( "material '%s:%s' invalid create stage group '%s'"
                    , _fileGroup->getName().c_str()
                    , _fileName.c_str()
                    , name.c_str()
                );

                return false;
            }

            m_materialStageIndexer.emplace( name, cache_stage );

            MapDefaultStagesName::const_iterator it_stage_found = m_defaultStagesEnum.find( name );

            if( it_stage_found != m_defaultStagesEnum.end() )
            {
                EMaterial materialId = it_stage_found->second;

                m_defaultStages[materialId] = cache_stage;
            }

            if( is_debug == true )
            {
                const RenderMaterialInterfacePtr & debugLineMaterial =
                    this->getMaterial( name, PT_LINELIST, 0, nullptr, MENGINE_DOCUMENT_FUNCTION );

                this->setDebugLineMaterial( debugLineMaterial );

                const RenderMaterialInterfacePtr & debugTriangleMaterial =
                    this->getMaterial( name, PT_TRIANGLELIST, 0, nullptr, MENGINE_DOCUMENT_FUNCTION );

                this->setDebugTriangleMaterial( debugTriangleMaterial );

            }
        }

        m_solidRenderMaterial[EMB_NORMAL] = m_defaultStages[EMB_NORMAL] != nullptr ? this->getMaterial3( EM_COLOR_BLEND, PT_TRIANGLELIST, 0, nullptr, MENGINE_DOCUMENT_FUNCTION ) : nullptr;
        m_solidRenderMaterial[EMB_ADD] = m_defaultStages[EMB_ADD] != nullptr ? this->getMaterial3( EM_COLOR_INTENSIVE, PT_TRIANGLELIST, 0, nullptr, MENGINE_DOCUMENT_FUNCTION ) : nullptr;
        m_solidRenderMaterial[EMB_SCREEN] = m_defaultStages[EMB_SCREEN] != nullptr ? this->getMaterial3( EM_COLOR_SCREEN, PT_TRIANGLELIST, 0, nullptr, MENGINE_DOCUMENT_FUNCTION ) : nullptr;
        m_solidRenderMaterial[EMB_MULTIPLY] = m_defaultStages[EMB_MULTIPLY] != nullptr ? this->getMaterial3( EM_COLOR_MULTIPLY, PT_TRIANGLELIST, 0, nullptr, MENGINE_DOCUMENT_FUNCTION ) : nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderMaterialService::unloadMaterials( const FileGroupInterfacePtr& _fileGroup, const FilePath& _fileName )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( _fileGroup, _fileName, &datablock, Metacode::Meta_Data::getVersion(), exist ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "materials '%s:%s' not found"
                    , _fileGroup->getName().c_str()
                    , _fileName.c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse materials '%s:%s'"
                    , _fileGroup->getName().c_str()
                    , _fileName.c_str()
                );
            }

            return false;
        }

        const ConstString & renderPlatformName = RENDER_SYSTEM()
            ->getRenderPlatformName();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_FragmentShader & includes_FragmentShader = datablock.get_Includes_FragmentShader();

        for( Metacode::Meta_Data::Meta_DataBlock::VectorMeta_FragmentShader::const_iterator
            it = includes_FragmentShader.begin(),
            it_end = includes_FragmentShader.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_FragmentShader & meta_FragmentShader = *it;

            const ConstString & renderPlatform = meta_FragmentShader.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            const ConstString & name = meta_FragmentShader.get_Name();

            m_fragmentShaders.erase( name );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_VertexShader & includes_VertexShader = datablock.get_Includes_VertexShader();

        for( Metacode::Meta_Data::Meta_DataBlock::VectorMeta_VertexShader::const_iterator
            it = includes_VertexShader.begin(),
            it_end = includes_VertexShader.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexShader & meta_VertexShader = *it;

            const ConstString & renderPlatform = meta_VertexShader.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            const ConstString & name = meta_VertexShader.get_Name();

            m_vertexShaders.erase( name );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Program & includes_Program = datablock.get_Includes_Program();

        for( Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Program::const_iterator
            it = includes_Program.begin(),
            it_end = includes_Program.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_Program & meta_Program = *it;

            ConstString renderPlatform;
            if( meta_Program.get_RenderPlatform( &renderPlatform ) == true )
            {
                if( renderPlatform != renderPlatformName )
                {
                    continue;
                }
            }

            const ConstString & name = meta_Program.get_Name();

            m_programs.erase( name );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Material & includes_Material = datablock.get_Includes_Material();

        for( Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Material::const_iterator
            it = includes_Material.begin(),
            it_end = includes_Material.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_Material & meta_Material = *it;

            const ConstString & name = meta_Material.get_Name();

            m_materialStageIndexer.erase( name );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_equalMaterial( const RenderMaterial * _material
        , EPrimitiveType _primitiveType
        , uint32_t _textureCount
        , const RenderTextureInterfacePtr * _textures
        , const RenderMaterialStage * _stage
    )
    {
        if( _material->getPrimitiveType() != _primitiveType )
        {
            return false;
        }

        if( _material->getTextureCount() != _textureCount )
        {
            return false;
        }

        for( uint32_t i = 0; i != _textureCount; ++i )
        {
            const RenderTextureInterfacePtr & texture = _material->getTexture( i );

            if( texture != _textures[i] )
            {
                return false;
            }
        }

        if( _material->getStage() != _stage )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderMaterialService::setDefaultTextureFilter( ETextureFilter _mipmap, ETextureFilter _magnification, ETextureFilter _minification )
    {
        m_defaultTextureFilterMipmap = _mipmap;
        m_defaultTextureFilterMagnification = _magnification;
        m_defaultTextureFilterMinification = _minification;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_equalTextureStage( const RenderTextureStage & _src, const RenderTextureStage & _dst )
    {
        if( _src.mipmap != _dst.mipmap )
        {
            return false;
        }

        if( _src.magnification != _dst.magnification )
        {
            return false;
        }

        if( _src.minification != _dst.minification )
        {
            return false;
        }

        if( _src.addressU != _dst.addressU )
        {
            return false;
        }

        if( _src.addressV != _dst.addressV )
        {
            return false;
        }

        if( _src.addressBorder != _dst.addressBorder )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_equalRenderStage( const RenderMaterialStage & _src, const RenderMaterialStage & _dst )
    {
        if( _src.program != _dst.program )
        {
            return false;
        }

        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            const RenderTextureStage & src_textureStage = _src.textureStage[i];
            const RenderTextureStage & dst_textureStage = _dst.textureStage[i];

            if( s_equalTextureStage( src_textureStage, dst_textureStage ) == false )
            {
                return false;
            }
        }

        if( _src.alphaBlendEnable != _dst.alphaBlendEnable )
        {
            return false;
        }

        if( _src.blendDst != _dst.blendDst )
        {
            return false;
        }

        if( _src.blendSrc != _dst.blendSrc )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialStage * RenderMaterialService::getMaterialStage( const ConstString & _materialName ) const
    {
        MapRenderStage::const_iterator it_found = m_materialStageIndexer.find( _materialName );

        if( it_found == m_materialStageIndexer.end() )
        {
            LOGGER_ERROR( "stage '%s' not found"
                , _materialName.c_str()
            );

            return nullptr;
        }

        const RenderMaterialStage * stage = it_found->second;

        return stage;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialStage * RenderMaterialService::cacheStage( const RenderMaterialStage & _other )
    {
        for( uint32_t it = 0; it != m_stageCount; ++it )
        {
            const RenderMaterialStage & self = m_stages[it];

            if( s_equalRenderStage( self, _other ) == false )
            {
                continue;
            }

            return &self;
        }

        if( m_stageCount == MENGINE_MATERIAL_RENDER_STAGE_MAX )
        {
            return nullptr;
        }

        m_stages[m_stageCount] = _other;

        RenderMaterialStage & cache_other = m_stages[m_stageCount];

        m_stageCount++;

        cache_other.id = m_stageCount;

        return &cache_other;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr RenderMaterialService::getMaterial( const ConstString & _materialName
        , EPrimitiveType _primitiveType
        , uint32_t _textureCount
        , const RenderTextureInterfacePtr * _textures, const Char * _doc )
    {
        MapRenderStage::const_iterator it_found = m_materialStageIndexer.find( _materialName );

        if( it_found == m_materialStageIndexer.end() )
        {
            LOGGER_ERROR( "stage '%s' not found"
                , _materialName.c_str()
            );

            return nullptr;
        }

        const RenderMaterialStage * stage = it_found->second;

        for( uint32_t i = 0; i != _textureCount; ++i )
        {
            if( _textures[i] == nullptr )
            {
                LOGGER_ERROR( "stage '%s' invalid setup texture %d"
                    , _materialName.c_str()
                    , i
                );

                return nullptr;
            }
        }

        RenderMaterialInterfacePtr material = this->getMaterial2( _materialName, stage, _primitiveType, _textureCount, _textures, _doc );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr RenderMaterialService::getMaterial2( const ConstString & _materialName
        , const RenderMaterialStage * _stage
        , EPrimitiveType _primitiveType
        , uint32_t _textureCount
        , const RenderTextureInterfacePtr * _textures, const Char * _doc )
    {
#ifdef MENGINE_DEBUG
        if( _stage == nullptr )
        {
            LOGGER_ERROR( "invalid get stage for material '%s'"
                , _materialName.c_str()
            );

            return nullptr;
        }
#endif

        uint32_t material_hash = this->makeMaterialHash( _materialName, _textureCount, _textures );

        uint32_t material_table_index = material_hash % MENGINE_RENDER_MATERIAL_HASH_TABLE_SIZE;

        IntrusiveListRenderMaterial & materials = m_materials[material_table_index];

        //for( const RenderMaterialPtr & material : materials )
        for( const RenderMaterial * material : materials )
        {
            uint32_t test_material_hash = material->getHash();

            if( test_material_hash != material_hash )
            {
                continue;
            }

            if( s_equalMaterial( material, _primitiveType, _textureCount, _textures, _stage ) == false )
            {
                continue;
            }

            return RenderMaterialInterfacePtr( material );
        }

        RenderMaterialPtr material = m_factoryMaterial->createObject( _doc );

        uint32_t id = this->makeMaterialIndex_();
        material->initialize( _materialName, id, material_hash, _primitiveType, _textureCount, _textures, _stage );

        materials.push_back( material.get() );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr RenderMaterialService::getMaterial3( EMaterial _materialId
        , EPrimitiveType _primitiveType
        , uint32_t _textureCount
        , const RenderTextureInterfacePtr * _textures, const Char * _doc )
    {
        const RenderMaterialStage * stage = m_defaultStages[_materialId];

        MENGINE_ASSERTION_MEMORY_PANIC( stage, nullptr, "invalid get stage for material '%s'"
            , m_defaultStageNames[_materialId].c_str()
            );

        const ConstString & materialName = m_defaultStageNames[_materialId];

        uint32_t material_hash = this->makeMaterialHash( materialName, _textureCount, _textures );

        uint32_t material_table_index = material_hash % MENGINE_RENDER_MATERIAL_HASH_TABLE_SIZE;

        IntrusiveListRenderMaterial & materials = m_materials[material_table_index];

        for( const RenderMaterial * material : materials )
        {
            uint32_t test_material_hash = material->getHash();

            if( test_material_hash != material_hash )
            {
                continue;
            }

            if( s_equalMaterial( material, _primitiveType, _textureCount, _textures, stage ) == false )
            {
                continue;
            }

            return RenderMaterialInterfacePtr( material );
        }

        RenderMaterialPtr material = m_factoryMaterial->createObject( _doc );

        uint32_t id = this->makeMaterialIndex_();
        material->initialize( materialName, id, material_hash, _primitiveType, _textureCount, _textures, stage );

        materials.push_back( material.get() );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialInterfacePtr & RenderMaterialService::getSolidMaterial( EMaterialBlendMode _blendMode ) const
    {
        const RenderMaterialInterfacePtr & material = m_solidRenderMaterial[_blendMode];

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderMaterialService::setDebugLineMaterial( const RenderMaterialInterfacePtr & _debugLineMaterial )
    {
        m_debugLineMaterial = _debugLineMaterial;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialInterfacePtr & RenderMaterialService::getDebugLineMaterial() const
    {
        return m_debugLineMaterial;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderMaterialService::setDebugTriangleMaterial( const RenderMaterialInterfacePtr & _debugTriangleMaterial )
    {
        m_debugTriangleMaterial = _debugTriangleMaterial;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialInterfacePtr & RenderMaterialService::getDebugTriangleMaterial() const
    {
        return m_debugTriangleMaterial;
    }
    //////////////////////////////////////////////////////////////////////////
    ETextureFilter RenderMaterialService::getDefaultTextureFilterMipmap() const
    {
        return m_defaultTextureFilterMipmap;
    }
    //////////////////////////////////////////////////////////////////////////
    ETextureFilter RenderMaterialService::getDefaultTextureFilterMagnification() const
    {
        return m_defaultTextureFilterMagnification;
    }
    //////////////////////////////////////////////////////////////////////////
    ETextureFilter RenderMaterialService::getDefaultTextureFilterMinification() const
    {
        return m_defaultTextureFilterMinification;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderMaterialService::onRenderMaterialDestroy_( RenderMaterial * _material )
    {
        uint32_t materialId = _material->getId();
        m_materialEnumerators.emplace_back( materialId );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialStage * RenderMaterialService::createRenderStageGroup( const ConstString & _name, const RenderMaterialStage & _stage )
    {
        MapRenderStage::const_iterator it_found = m_materialStageIndexer.find( _name );

        if( it_found != m_materialStageIndexer.end() )
        {
            LOGGER_ERROR( "'%s' is already created"
                , _name.c_str()
            );

            return nullptr;
        }

        const RenderMaterialStage * cache_stage = this->cacheStage( _stage );

        if( cache_stage == nullptr )
        {
            LOGGER_ERROR( "'%s' invalid cache"
                , _name.c_str()
            );

            return nullptr;
        }

        return cache_stage;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t RenderMaterialService::makeMaterialIndex_()
    {
        if( m_materialEnumerators.empty() == false )
        {
            uint32_t id = m_materialEnumerators.back();
            m_materialEnumerators.pop_back();

            return id;
        }

        return ++m_materialEnumerator;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t RenderMaterialService::makeMaterialHash( const ConstString & _materialName, uint32_t _textureCount, const RenderTextureInterfacePtr * _textures ) const
    {
        uint32_t material_hash = (uint32_t)_materialName.hash();

        for( uint32_t i = 0; i != _textureCount; ++i )
        {
            uint32_t texture_id = _textures[i]->getId();

            material_hash += texture_id + i * 3571;
        }

        return material_hash;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr RenderMaterialService::createVertexAttribute_( const ConstString & _name, uint32_t _elementSize )
    {
        RenderVertexAttributeInterfacePtr vertexAttribute = RENDER_SYSTEM()
            ->createVertexAttribute( _name, _elementSize, MENGINE_DOCUMENT_FUNCTION );

        return vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr RenderMaterialService::createVertexShader_( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _compile )
    {
        MemoryInterfacePtr memory = Helper::createMemoryFile( _fileGroup, _filePath, false, "RenderMaterialService::createVertexShader_", __FILE__, __LINE__ );

        if( memory == nullptr )
        {
            return nullptr;
        }

        RenderVertexShaderInterfacePtr shader = RENDER_SYSTEM()
            ->createVertexShader( _name, memory, _compile, MENGINE_DOCUMENT_FUNCTION );

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr RenderMaterialService::createFragmentShader_( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _compile )
    {
        MemoryInterfacePtr memory = Helper::createMemoryFile( _fileGroup, _filePath, false, "RenderMaterialService::createFragmentShader_", __FILE__, __LINE__ );

        if( memory == nullptr )
        {
            return nullptr;
        }

        RenderFragmentShaderInterfacePtr shader = RENDER_SYSTEM()
            ->createFragmentShader( _name, memory, _compile, MENGINE_DOCUMENT_FUNCTION );

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexShaderInterfacePtr & RenderMaterialService::getVertexShader( const ConstString & _name ) const
    {
        MapRenderVertexShaders::const_iterator it_found = m_vertexShaders.find( _name );

        if( it_found == m_vertexShaders.end() )
        {
            return RenderVertexShaderInterfacePtr::none();
        }

        const RenderVertexShaderInterfacePtr & shader = it_found->second;

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderFragmentShaderInterfacePtr & RenderMaterialService::getFragmentShader( const ConstString & _name ) const
    {
        MapRenderFragmentShaders::const_iterator it_found = m_fragmentShaders.find( _name );

        if( it_found == m_fragmentShaders.end() )
        {
            return RenderFragmentShaderInterfacePtr::none();
        }

        const RenderFragmentShaderInterfacePtr & shader = it_found->second;

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexAttributeInterfacePtr & RenderMaterialService::getVertexAttribute( const ConstString & _name ) const
    {
        MapRenderVertexAttributes::const_iterator it_found = m_vertexAttributes.find( _name );

        if( it_found == m_vertexAttributes.end() )
        {
            return RenderVertexAttributeInterfacePtr::none();
        }

        const RenderVertexAttributeInterfacePtr & vertexAttribute = it_found->second;

        return vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderProgramInterfacePtr & RenderMaterialService::getProgram( const ConstString & _name ) const
    {
        MapRenderPrograms::const_iterator it_found = m_programs.find( _name );

        if( it_found == m_programs.end() )
        {
            return RenderProgramInterfacePtr::none();
        }

        const RenderProgramInterfacePtr & program = it_found->second;

        return program;
    }
}