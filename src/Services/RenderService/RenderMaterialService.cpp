#include "RenderMaterialService.h"

#include "Interface/ConfigServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderMaterialService, Mengine::RenderMaterialService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
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
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialService::RenderMaterialService()
        : m_materialEnumerator( 0 )
        , m_defaultTextureFilterMipmap( TF_NONE )
        , m_defaultTextureFilterMagnification( TF_LINEAR )
        , m_defaultTextureFilterMinification( TF_LINEAR )
        , m_stageCount( 0 )
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
        m_defaultStageNames[EM_COLOR_BLEND_PREMULTIPLY] = STRINGIZE_STRING_LOCAL( "Color_Blend_Premultiply" );
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

        if( defaultTextureFilterMipmap >= __MAX_TEXTURE_FILTER__ )
        {
            LOGGER_ERROR( "[DefaultTextureFilterMipmap] undefined great %u current '%u'"
                , __MAX_TEXTURE_FILTER__
                , defaultTextureFilterMipmap
            );

            return false;
        }

        if( defaultTextureFilterMagnification >= __MAX_TEXTURE_FILTER__ )
        {
            LOGGER_ERROR( "[DefaultTextureFilterMagnification] undefined great %u current '%u'"
                , __MAX_TEXTURE_FILTER__
                , defaultTextureFilterMagnification
            );

            return false;
        }

        m_defaultTextureFilterMipmap = Detail::parseConfigTextureFilterValue( defaultTextureFilterMipmap );
        m_defaultTextureFilterMagnification = Detail::parseConfigTextureFilterValue( defaultTextureFilterMagnification );
        m_defaultTextureFilterMinification = Detail::parseConfigTextureFilterValue( defaultTextureFilterMinification );

        m_factoryMaterial = Helper::makeFactoryPoolWithListener<RenderMaterial, 256>( this, &RenderMaterialService::onRenderMaterialDestroy_, MENGINE_DOCUMENT_FACTORABLE );

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

        m_solidRenderMaterial[0][0] = nullptr;
        m_solidRenderMaterial[1][0] = nullptr;
        m_solidRenderMaterial[2][0] = nullptr;
        m_solidRenderMaterial[3][0] = nullptr;

        m_solidRenderMaterial[0][1] = nullptr;
        m_solidRenderMaterial[1][1] = nullptr;
        m_solidRenderMaterial[2][1] = nullptr;
        m_solidRenderMaterial[3][1] = nullptr;


        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMaterial );

        m_factoryMaterial = nullptr;
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

        if( _src.blendSrc != _dst.blendSrc )
        {
            return false;
        }

        if( _src.blendDst != _dst.blendDst )
        {
            return false;
        }

        if( _src.blendOp != _dst.blendOp )
        {
            return false;
        }

        if( _src.separateAlphaBlendSrc != _dst.separateAlphaBlendSrc )
        {
            return false;
        }

        if( _src.separateAlphaBlendDst != _dst.separateAlphaBlendDst )
        {
            return false;
        }

        if( _src.separateAlphaBlendOp != _dst.separateAlphaBlendOp )
        {
            return false;
        }

        if( _src.separateAlphaBlendEnable != _dst.separateAlphaBlendEnable )
        {
            return false;
        }

        if( _src.alphaBlendEnable != _dst.alphaBlendEnable )
        {
            return false;
        }

        if( _src.depthBufferTestEnable != _dst.depthBufferTestEnable )
        {
            return false;
        }

        if( _src.depthBufferWriteEnable != _dst.depthBufferWriteEnable )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialStage * RenderMaterialService::removeMaterialStage( const ConstString & _name )
    {
        MENGINE_ASSERTION_FATAL( m_materialStageIndexer.exist( _name ) == true, "absent material stage '%s'"
            , _name.c_str()
        );

        const RenderMaterialStage * stage = m_materialStageIndexer.erase( _name );

        return stage;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialStage * RenderMaterialService::getMaterialStage( const ConstString & _name ) const
    {
        const RenderMaterialStage * stage = m_materialStageIndexer.find( _name );

        if( stage == nullptr )
        {
            LOGGER_ERROR( "stage '%s' not found"
                , _name.c_str()
            );

            return nullptr;
        }

        return stage;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialStage * RenderMaterialService::cacheMaterialStage( const RenderMaterialStage & _stage )
    {
        for( uint32_t it = 0; it != m_stageCount; ++it )
        {
            const RenderMaterialStage & self = m_stages[it];

            if( s_equalRenderStage( self, _stage ) == false )
            {
                continue;
            }

            return &self;
        }

        if( m_stageCount == MENGINE_MATERIAL_RENDER_STAGE_MAX )
        {
            return nullptr;
        }

        m_stages[m_stageCount] = _stage;

        RenderMaterialStage & cache_stage = m_stages[m_stageCount];

        m_stageCount++;

        cache_stage.id = m_stageCount;
        cache_stage.cache = RENDER_SYSTEM()
            ->createRenderMaterialStageCache( &cache_stage );

        return &cache_stage;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderMaterialService::uncacheMaterialStage( const RenderMaterialStage * _stage )
    {
        for( uint32_t it = 0; it != m_stageCount; ++it )
        {
            RenderMaterialStage * self = m_stages + it;

            if( _stage != self )
            {
                continue;
            }

            self->program = nullptr;

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderMaterialService::updateSolidRenderMaterial()
    {
        m_solidRenderMaterial[EMB_NORMAL][0] = m_defaultStages[EMB_NORMAL] != nullptr ? this->getMaterial3( EM_COLOR_BLEND, PT_TRIANGLELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE ) : nullptr;
        m_solidRenderMaterial[EMB_ADD][0] = m_defaultStages[EMB_ADD] != nullptr ? this->getMaterial3( EM_COLOR_INTENSIVE, PT_TRIANGLELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE ) : nullptr;
        m_solidRenderMaterial[EMB_SCREEN][0] = m_defaultStages[EMB_SCREEN] != nullptr ? this->getMaterial3( EM_COLOR_SCREEN, PT_TRIANGLELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE ) : nullptr;
        m_solidRenderMaterial[EMB_MULTIPLY][0] = m_defaultStages[EMB_MULTIPLY] != nullptr ? this->getMaterial3( EM_COLOR_MULTIPLY, PT_TRIANGLELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE ) : nullptr;

        m_solidRenderMaterial[EMB_NORMAL][1] = m_defaultStages[EMB_NORMAL] != nullptr ? this->getMaterial3( EM_COLOR_BLEND, PT_TRIANGLELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE ) : nullptr;
        m_solidRenderMaterial[EMB_ADD][1] = m_defaultStages[EMB_ADD] != nullptr ? this->getMaterial3( EM_COLOR_INTENSIVE, PT_TRIANGLELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE ) : nullptr;
        m_solidRenderMaterial[EMB_SCREEN][1] = m_defaultStages[EMB_SCREEN] != nullptr ? this->getMaterial3( EM_COLOR_SCREEN, PT_TRIANGLELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE ) : nullptr;
        m_solidRenderMaterial[EMB_MULTIPLY][1] = m_defaultStages[EMB_MULTIPLY] != nullptr ? this->getMaterial3( EM_COLOR_MULTIPLY, PT_TRIANGLELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE ) : nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr RenderMaterialService::getMaterial( const ConstString & _materialName
        , EPrimitiveType _primitiveType
        , const RenderTextureInterfacePtr * _textures
        , uint32_t _textureCount, const DocumentPtr & _doc )
    {
        const RenderMaterialStage * stage = m_materialStageIndexer.find( _materialName );

        if( stage == nullptr )
        {
            LOGGER_ERROR( "stage '%s' not found"
                , _materialName.c_str()
            );

            return nullptr;
        }

#ifdef MENGINE_DEBUG
        for( uint32_t i = 0; i != _textureCount; ++i )
        {
            if( _textures[i] == nullptr )
            {
                LOGGER_ERROR( "stage '%s' invalid setup texture %u"
                    , _materialName.c_str()
                    , i
                );

                return nullptr;
            }
        }
#endif

        RenderMaterialInterfacePtr material = this->getMaterial2( _materialName, stage, _primitiveType, _textures, _textureCount, _doc );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr RenderMaterialService::getMaterial2( const ConstString & _materialName
        , const RenderMaterialStage * _stage
        , EPrimitiveType _primitiveType
        , const RenderTextureInterfacePtr * _textures
        , uint32_t _textureCount, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stage, "invalid get stage for material '%s'"
            , _materialName.c_str()
        );

        uint32_t material_hash = this->makeMaterialHash_( _materialName, _textureCount, _textures );

        uint32_t material_table_index = material_hash % MENGINE_RENDER_MATERIAL_HASH_TABLE_SIZE;

        IntrusiveListRenderMaterial & materials = m_materials[material_table_index];

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

        MENGINE_ASSERTION_MEMORY_PANIC( material );

        uint32_t id = this->makeMaterialIndex_();
        material->initialize( _materialName, id, material_hash, _primitiveType, _textureCount, _textures, _stage );

        materials.push_back( material.get() );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr RenderMaterialService::getMaterial3( EMaterial _materialId
        , EPrimitiveType _primitiveType
        , const RenderTextureInterfacePtr * _textures
        , uint32_t _textureCount, const DocumentPtr & _doc )
    {
        const RenderMaterialStage * stage = m_defaultStages[_materialId];

        MENGINE_ASSERTION_MEMORY_PANIC( stage, "invalid get stage for material '%s'"
            , m_defaultStageNames[_materialId].c_str()
        );

        const ConstString & materialName = m_defaultStageNames[_materialId];

        uint32_t material_hash = this->makeMaterialHash_( materialName, _textureCount, _textures );

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

        MENGINE_ASSERTION_MEMORY_PANIC( material );

        uint32_t id = this->makeMaterialIndex_();
        material->initialize( materialName, id, material_hash, _primitiveType, _textureCount, _textures, stage );

        materials.push_back( material.get() );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialInterfacePtr & RenderMaterialService::getSolidMaterial( EMaterialBlendMode _blendMode, bool _premultiply ) const
    {
        const RenderMaterialInterfacePtr & material = m_solidRenderMaterial[_blendMode][_premultiply == true ? 1 : 0];

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
    const RenderMaterialStage * RenderMaterialService::createMaterialStage( const ConstString & _name, const RenderMaterialStage & _stage )
    {
        MENGINE_ASSERTION( m_materialStageIndexer.find( _name ) == nullptr, "'%s' is already created"
            , _name.c_str()
        );

        const RenderMaterialStage * cache_stage = this->cacheMaterialStage( _stage );

        MENGINE_ASSERTION_MEMORY_PANIC( cache_stage, "'%s' invalid cache"
            , _name.c_str()
        );

        m_materialStageIndexer.emplace( _name, cache_stage );

        MapDefaultStagesName::const_iterator it_stage_found = m_defaultStagesEnum.find( _name );

        if( it_stage_found != m_defaultStagesEnum.end() )
        {
            EMaterial materialId = it_stage_found->second;

            m_defaultStages[materialId] = cache_stage;
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

        uint32_t new_id = ++m_materialEnumerator;

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t RenderMaterialService::makeMaterialHash_( const ConstString & _materialName, uint32_t _textureCount, const RenderTextureInterfacePtr * _textures ) const
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
    RenderVertexAttributeInterfacePtr RenderMaterialService::createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( m_vertexAttributes.exist( _name ) == false, "already has vertex attribute '%s' (doc: %s)"
            , _name.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        RenderVertexAttributeInterfacePtr vertexAttribute = RENDER_SYSTEM()
            ->createVertexAttribute( _name, _elementSize, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( vertexAttribute );

        m_vertexAttributes.emplace( _name, vertexAttribute );

        return vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr RenderMaterialService::removeVertexAttribute( const ConstString & _name )
    {
        MENGINE_ASSERTION_FATAL( m_vertexAttributes.exist( _name ) == true, "absent vertex attribute '%s'"
            , _name.c_str()
        );

        RenderVertexAttributeInterfacePtr vertexAttribute = m_vertexAttributes.erase( _name );

        return vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr RenderMaterialService::createVertexShader( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _converterType, bool _compile, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _converterType );

        MENGINE_ASSERTION_FATAL( m_vertexShaders.exist( _name ) == false, "material '%s:%s' already has vertex shader '%s' (doc: %s)"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
            , _name.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        FilePath outFilePath = _filePath;

#ifndef MENGINE_MASTER_RELEASE
        if( CONVERTER_SERVICE()
            ->convert( _converterType, _fileGroup, _filePath, &outFilePath, _doc ) == false )
        {
            return nullptr;
        }
#endif

        MemoryInterfacePtr memory = Helper::createMemoryFile( _fileGroup, outFilePath, false, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        RenderVertexShaderInterfacePtr vertexShader = RENDER_SYSTEM()
            ->createVertexShader( _name, memory, _compile, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( vertexShader );

        m_vertexShaders.emplace( _name, vertexShader );

        return vertexShader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr RenderMaterialService::removeVertexShader( const ConstString & _name )
    {
        MENGINE_ASSERTION_FATAL( m_vertexShaders.exist( _name ) == true, "absent vertex shader '%s'"
            , _name.c_str()
        );

        RenderVertexShaderInterfacePtr vertexShader = m_vertexShaders.erase( _name );

        return vertexShader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr RenderMaterialService::createFragmentShader( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _converterType, bool _compile, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _converterType );

        MENGINE_ASSERTION_FATAL( m_fragmentShaders.exist( _name ) == false, "material '%s:%s' already has fragment shader '%s' (doc: %s)"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
            , _name.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        FilePath outFilePath = _filePath;

#ifndef MENGINE_MASTER_RELEASE
        if( CONVERTER_SERVICE()
            ->convert( _converterType, _fileGroup, _filePath, &outFilePath, _doc ) == false )
        {
            return nullptr;
        }
#endif

        MemoryInterfacePtr memory = Helper::createMemoryFile( _fileGroup, outFilePath, false, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        RenderFragmentShaderInterfacePtr fragmentShader = RENDER_SYSTEM()
            ->createFragmentShader( _name, memory, _compile, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( fragmentShader );

        m_fragmentShaders.emplace( _name, fragmentShader );

        return fragmentShader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr RenderMaterialService::removeFragmentShader( const ConstString & _name )
    {
        MENGINE_ASSERTION_FATAL( m_fragmentShaders.exist( _name ) == true, "absent fragment shader '%s'"
            , _name.c_str()
        );

        RenderFragmentShaderInterfacePtr fragmentShader = m_fragmentShaders.erase( _name );

        return fragmentShader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramInterfacePtr RenderMaterialService::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertexShader, const RenderFragmentShaderInterfacePtr & _fragmentShader, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( m_programs.exist( _name ) == false, "already has program '%s' (doc: %s)"
            , _name.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        RenderProgramInterfacePtr program = RENDER_SYSTEM()
            ->createProgram( _name, _vertexShader, _fragmentShader, _vertexAttribute, _samplerCount, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( program );

        m_programs.emplace( _name, program );

        return program;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramInterfacePtr RenderMaterialService::removeProgram( const ConstString & _name )
    {
        MENGINE_ASSERTION_FATAL( m_programs.exist( _name ) == true, "absent program '%s'"
            , _name.c_str()
        );

        RenderProgramInterfacePtr program = m_programs.erase( _name );

        return program;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexShaderInterfacePtr & RenderMaterialService::getVertexShader( const ConstString & _name ) const
    {
        const RenderVertexShaderInterfacePtr & shader = m_vertexShaders.find( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( shader, "not found vertex shader '%s'"
            , _name.c_str()
        );

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderFragmentShaderInterfacePtr & RenderMaterialService::getFragmentShader( const ConstString & _name ) const
    {
        const RenderFragmentShaderInterfacePtr & shader = m_fragmentShaders.find( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( shader, "not found fragment shader '%s'"
            , _name.c_str()
        );

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexAttributeInterfacePtr & RenderMaterialService::getVertexAttribute( const ConstString & _name ) const
    {
        const RenderVertexAttributeInterfacePtr & vertexAttribute = m_vertexAttributes.find( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( vertexAttribute, "not found vertex attribute '%s'"
            , _name.c_str()
        );

        return vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderProgramInterfacePtr & RenderMaterialService::getProgram( const ConstString & _name ) const
    {
        const RenderProgramInterfacePtr & program = m_programs.find( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( program, "not found program '%s'"
            , _name.c_str()
        );

        return program;
    }
    //////////////////////////////////////////////////////////////////////////
}
