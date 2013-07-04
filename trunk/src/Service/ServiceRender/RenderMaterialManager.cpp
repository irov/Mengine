#   include "RenderMaterialManager.h"

#   include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderMaterialManager, Menge::RenderMaterialServiceInterface, Menge::RenderMaterialManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////    
    RenderMaterialManager::RenderMaterialManager()
        : m_serviceProvider(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    RenderMaterialManager::~RenderMaterialManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    void RenderMaterialManager::setServiceProvider( ServiceProviderInterface * _provider )
    {
        m_serviceProvider = _provider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * RenderMaterialManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderMaterialManager::initialize()
    {
        {
            RenderMaterial mt;

            mt.alphaBlendEnable = true;
            mt.alphaTestEnable = true;
            mt.depthBufferWriteEnable = false;

            mt.blendSrc = BF_SOURCE_ALPHA;
            mt.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            mt.textureStage[0].alphaOp = TOP_MODULATE;
            mt.textureStage[0].colorOp = TOP_MODULATE;

            this->createMaterialGroup( Helper::stringizeString(m_serviceProvider, "BlendSprite"), mt );
        }

        {
            RenderMaterial mt;

            mt.alphaBlendEnable = false;
            mt.alphaTestEnable = false;
            mt.depthBufferWriteEnable = false;

            mt.blendSrc = BF_SOURCE_ALPHA;
            mt.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            mt.textureStage[0].alphaOp = TOP_MODULATE;
            mt.textureStage[0].colorOp = TOP_MODULATE;

            this->createMaterialGroup( Helper::stringizeString(m_serviceProvider, "SolidSprite"), mt );
        }

        {
            RenderMaterial mt;

            mt.alphaBlendEnable = true;
            mt.alphaTestEnable = true;
            mt.depthBufferWriteEnable = false;

            mt.blendSrc = BF_SOURCE_ALPHA;
            mt.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            mt.textureStage[0].alphaOp = TOP_MODULATE;
            mt.textureStage[0].colorOp = TOP_SELECTARG1;

            this->createMaterialGroup( Helper::stringizeString(m_serviceProvider, "Accumulator"), mt );
        }

        {
            RenderMaterial mt;

            mt.alphaBlendEnable = true;
            mt.alphaTestEnable = true;
            mt.depthBufferWriteEnable = false;

            mt.blendSrc = BF_SOURCE_ALPHA;
            mt.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            RenderTextureStage & ts0 = mt.textureStage[0];

            ts0.colorOp = TOP_MODULATE;
            ts0.colorArg1 = TARG_TEXTURE;
            ts0.colorArg2 = TARG_DIFFUSE;
            ts0.alphaOp = TOP_SELECTARG1;
            ts0.alphaArg1 = TARG_DIFFUSE;

            RenderTextureStage & ts1 = mt.textureStage[1];

            ts1.colorOp = TOP_SELECTARG1;
            ts1.colorArg1 = TARG_CURRENT;
            ts1.alphaOp = TOP_MODULATE;
            ts1.alphaArg1 = TARG_TEXTURE;
            ts1.alphaArg2 = TARG_CURRENT;
            ts1.texCoordIndex = 1;

            this->createMaterialGroup( Helper::stringizeString(m_serviceProvider, "ExternalAlpha"), mt );
        }

        {
            RenderMaterial mt;

            mt.alphaBlendEnable = true;
            mt.alphaTestEnable = true;
            mt.depthBufferWriteEnable = false;

            mt.blendSrc = BF_SOURCE_ALPHA;
            mt.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            RenderTextureStage & ts0 = mt.textureStage[0];

            ts0.colorOp = TOP_SELECTARG1;
            ts0.colorArg1 = TARG_DIFFUSE;
            //ts0.colorArg2 = TARG_DIFFUSE;
            ts0.alphaOp = TOP_SELECTARG1;
            ts0.alphaArg1 = TARG_DIFFUSE;

            RenderTextureStage & ts1 = mt.textureStage[1];

            ts1.colorOp = TOP_SELECTARG1;
            ts1.colorArg1 = TARG_CURRENT;
            ts1.alphaOp = TOP_MODULATE;
            ts1.alphaArg1 = TARG_TEXTURE;
            ts1.alphaArg2 = TARG_CURRENT;
            ts1.texCoordIndex = 1;

            this->createMaterialGroup( Helper::stringizeString(m_serviceProvider, "ExternalAlpha_OnlyColor"), mt );
        }

        {
            RenderMaterial mt;

            mt.alphaBlendEnable = true;
            mt.alphaTestEnable = true;
            mt.depthBufferWriteEnable = false;

            mt.blendSrc = BF_SOURCE_ALPHA;
            mt.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            mt.textureStage[0].alphaOp = TOP_MODULATE;
            mt.textureStage[0].colorOp = TOP_SELECTARG2;

            this->createMaterialGroup( Helper::stringizeString(m_serviceProvider, "OnlyColor"), mt );
        }

        {
            RenderMaterial mt;

            mt.alphaBlendEnable = false;
            mt.alphaTestEnable = false;
            mt.depthBufferWriteEnable = false;

            mt.blendSrc = BF_SOURCE_ALPHA;
            mt.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            mt.textureStage[0].colorOp = TOP_SELECTARG2;
            mt.textureStage[0].alphaOp = TOP_SELECTARG2;

            this->createMaterialGroup( Helper::stringizeString(m_serviceProvider, "Debug"), mt );
        }

        {
            RenderMaterial mt;

            mt.alphaBlendEnable = true;
            mt.alphaTestEnable = false;
            mt.depthBufferWriteEnable = false;

            mt.blendSrc = BF_SOURCE_ALPHA;
            mt.blendDst = BF_ONE;

            mt.textureStage[0].colorOp = TOP_MODULATE;
            mt.textureStage[0].alphaOp = TOP_MODULATE;

            this->createMaterialGroup( Helper::stringizeString(m_serviceProvider, "ParticleIntensive"), mt );
        }

        {
            RenderMaterial mt;

            mt.alphaBlendEnable = true;
            mt.alphaTestEnable = false;
            mt.depthBufferWriteEnable = false;

            mt.blendSrc = BF_SOURCE_ALPHA;
            mt.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            mt.textureStage[0].colorOp = TOP_MODULATE;
            mt.textureStage[0].alphaOp = TOP_MODULATE;

            this->createMaterialGroup( Helper::stringizeString(m_serviceProvider, "ParticleBlend"), mt );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderMaterialManager::finalize()
    {
        for( TMapMaterialGroup::iterator
            it = m_mapMaterialGroup.begin(),
            it_end = m_mapMaterialGroup.end();
        it != it_end;
        ++it )
        {
            delete it->second;
        }

        m_mapMaterialGroup.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderMaterialManager::loadMaterials( const ConstString& _pakName, const FilePath& _filename )
    {
        (void)_pakName;
        (void)_filename;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialGroup * RenderMaterialManager::getMaterialGroup( const ConstString & _name ) const
    {
        TMapMaterialGroup::const_iterator it_found = m_mapMaterialGroup.find( _name );

        if( it_found == m_mapMaterialGroup.end() )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::getMaterialGroup: not exsist RenderMaterial '%s'"
                , _name.c_str()
                );

            return nullptr;
        }

        RenderMaterialGroup * materialGroup = it_found->second;

        return materialGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderMaterialManager::createMaterialGroup( const ConstString & _name, const RenderMaterial & _material )
    {
        TMapMaterialGroup::iterator it_found = m_mapMaterialGroup.find( _name );

        if( it_found != m_mapMaterialGroup.end() )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::createMaterialGroup: MaterialGroup '%s' is already created!"
                , _name.c_str()
                );

            return false;
        }

        RenderMaterialGroup * materialGroup = new RenderMaterialGroup;

        materialGroup->filter_group[0] = _material;

        for( size_t stageId = 0; stageId != MENGE_MAX_TEXTURE_STAGES; ++stageId )
        {
            RenderTextureStage & stage = materialGroup->filter_group[0].textureStage[stageId];

            stage.addressU = TAM_CLAMP;
            stage.addressV = TAM_CLAMP;
            stage.filter = TF_ANISOTROPIC;
        }

        materialGroup->filter_group[1] = _material;

        for( size_t stageId = 0; stageId != MENGE_MAX_TEXTURE_STAGES; ++stageId )
        {
            RenderTextureStage & stage = materialGroup->filter_group[1].textureStage[stageId];

            stage.addressU = TAM_WRAP;
            stage.addressV = TAM_CLAMP;
            stage.filter = TF_ANISOTROPIC;
        }

        materialGroup->filter_group[2] = _material;

        for( size_t stageId = 0; stageId != MENGE_MAX_TEXTURE_STAGES; ++stageId )
        {
            RenderTextureStage & stage = materialGroup->filter_group[2].textureStage[stageId];

            stage.addressU = TAM_CLAMP;
            stage.addressV = TAM_WRAP;
            stage.filter = TF_ANISOTROPIC;
        }

        materialGroup->filter_group[3] = _material;

        for( size_t stageId = 0; stageId != MENGE_MAX_TEXTURE_STAGES; ++stageId )
        {
            RenderTextureStage & stage = materialGroup->filter_group[2].textureStage[stageId];

            stage.addressU = TAM_WRAP;
            stage.addressV = TAM_WRAP;
            stage.filter = TF_ANISOTROPIC;
        }

        materialGroup->group[0] = _material;

        for( size_t stageId = 0; stageId != MENGE_MAX_TEXTURE_STAGES; ++stageId )
        {
            RenderTextureStage & stage = materialGroup->group[0].textureStage[stageId];
            
            stage.addressU = TAM_CLAMP;
            stage.addressV = TAM_CLAMP;
            stage.filter = TF_LINEAR;
        }

        materialGroup->group[1] = _material;

        for( size_t stageId = 0; stageId != MENGE_MAX_TEXTURE_STAGES; ++stageId )
        {
            RenderTextureStage & stage = materialGroup->group[1].textureStage[stageId];

            stage.addressU = TAM_WRAP;
            stage.addressV = TAM_CLAMP;
            stage.filter = TF_LINEAR;
        }

        materialGroup->group[2] = _material;

        for( size_t stageId = 0; stageId != MENGE_MAX_TEXTURE_STAGES; ++stageId )
        {
            RenderTextureStage & stage = materialGroup->group[2].textureStage[stageId];

            stage.addressU = TAM_CLAMP;
            stage.addressV = TAM_WRAP;
            stage.filter = TF_LINEAR;
        }

        materialGroup->group[3] = _material;

        for( size_t stageId = 0; stageId != MENGE_MAX_TEXTURE_STAGES; ++stageId )
        {
            RenderTextureStage & stage = materialGroup->group[3].textureStage[stageId];

            stage.addressU = TAM_WRAP;
            stage.addressV = TAM_WRAP;
            stage.filter = TF_LINEAR;
        }        

        m_mapMaterialGroup.insert( std::make_pair(_name, materialGroup) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderMaterialManager::removeMaterialGroup( const ConstString & _name )
    {
        TMapMaterialGroup::iterator it_found = m_mapMaterialGroup.find( _name );

        if( it_found == m_mapMaterialGroup.end() )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::removeMaterialGroup: not exsist RenderMaterial '%s'"
                , _name.c_str()
                );

            return;
        }

        delete it_found->second;

        m_mapMaterialGroup.erase( it_found );
    }
}