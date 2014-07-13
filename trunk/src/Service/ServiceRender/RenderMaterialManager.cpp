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
		, m_materialEnumerator(0)
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
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            rs.textureStage[0].alphaOp = TOP_MODULATE;
            rs.textureStage[0].colorOp = TOP_MODULATE;

            this->createRenderStageGroup( CONST_STRING_LOCAL(m_serviceProvider, "BlendSprite"), rs );
		}

        {
            RenderStage rs;

            rs.alphaBlendEnable = false;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            rs.textureStage[0].alphaOp = TOP_MODULATE;
            rs.textureStage[0].colorOp = TOP_MODULATE;

            this->createRenderStageGroup( CONST_STRING_LOCAL(m_serviceProvider, "SolidSprite"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            rs.textureStage[0].alphaOp = TOP_MODULATE;
            rs.textureStage[0].colorOp = TOP_SELECTARG1;

            this->createRenderStageGroup( CONST_STRING_LOCAL(m_serviceProvider, "Accumulator"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            RenderTextureStage & ts0 = rs.textureStage[0];

            ts0.colorOp = TOP_MODULATE;
            ts0.colorArg1 = TARG_TEXTURE;
            ts0.colorArg2 = TARG_DIFFUSE;
            ts0.alphaOp = TOP_SELECTARG1;
            ts0.alphaArg1 = TARG_DIFFUSE;

            RenderTextureStage & ts1 = rs.textureStage[1];

            ts1.colorOp = TOP_SELECTARG1;
            ts1.colorArg1 = TARG_CURRENT;
            ts1.alphaOp = TOP_MODULATE;
            ts1.alphaArg1 = TARG_TEXTURE;
            ts1.alphaArg2 = TARG_CURRENT;
            ts1.texCoordIndex = 1;

            this->createRenderStageGroup( CONST_STRING_LOCAL(m_serviceProvider, "ExternalAlpha"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE;

            RenderTextureStage & ts0 = rs.textureStage[0];

            ts0.colorOp = TOP_MODULATE;
            ts0.colorArg1 = TARG_TEXTURE;
            ts0.colorArg2 = TARG_DIFFUSE;
            ts0.alphaOp = TOP_SELECTARG1;
            ts0.alphaArg1 = TARG_DIFFUSE;

            RenderTextureStage & ts1 = rs.textureStage[1];

            ts1.colorOp = TOP_SELECTARG1;
            ts1.colorArg1 = TARG_CURRENT;
            ts1.alphaOp = TOP_MODULATE;
            ts1.alphaArg1 = TARG_TEXTURE;
            ts1.alphaArg2 = TARG_CURRENT;
            ts1.texCoordIndex = 1;

            this->createRenderStageGroup( CONST_STRING_LOCAL(m_serviceProvider, "ExternalAlphaIntensive"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            RenderTextureStage & ts0 = rs.textureStage[0];

            ts0.colorOp = TOP_SELECTARG1;
            ts0.colorArg1 = TARG_DIFFUSE;
            //ts0.colorArg2 = TARG_DIFFUSE;
            ts0.alphaOp = TOP_SELECTARG1;
            ts0.alphaArg1 = TARG_DIFFUSE;

            RenderTextureStage & ts1 = rs.textureStage[1];

            ts1.colorOp = TOP_SELECTARG1;
            ts1.colorArg1 = TARG_CURRENT;
            ts1.alphaOp = TOP_MODULATE;
            ts1.alphaArg1 = TARG_TEXTURE;
            ts1.alphaArg2 = TARG_CURRENT;
            ts1.texCoordIndex = 1;

            this->createRenderStageGroup( CONST_STRING_LOCAL(m_serviceProvider, "ExternalAlpha_OnlyColor"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE;

            RenderTextureStage & ts0 = rs.textureStage[0];

            ts0.colorOp = TOP_SELECTARG1;
            ts0.colorArg1 = TARG_DIFFUSE;
            //ts0.colorArg2 = TARG_DIFFUSE;
            ts0.alphaOp = TOP_SELECTARG1;
            ts0.alphaArg1 = TARG_DIFFUSE;

            RenderTextureStage & ts1 = rs.textureStage[1];

            ts1.colorOp = TOP_SELECTARG1;
            ts1.colorArg1 = TARG_CURRENT;
            ts1.alphaOp = TOP_MODULATE;
            ts1.alphaArg1 = TARG_TEXTURE;
            ts1.alphaArg2 = TARG_CURRENT;
            ts1.texCoordIndex = 1;

            this->createRenderStageGroup( CONST_STRING_LOCAL(m_serviceProvider, "ExternalAlphaIntensive_OnlyColor"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            rs.textureStage[0].alphaOp = TOP_MODULATE;
            rs.textureStage[0].colorOp = TOP_SELECTARG2;

            this->createRenderStageGroup( CONST_STRING_LOCAL(m_serviceProvider, "OnlyColor"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = false;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            rs.textureStage[0].colorOp = TOP_SELECTARG2;
            rs.textureStage[0].alphaOp = TOP_SELECTARG2;

            this->createRenderStageGroup( CONST_STRING_LOCAL(m_serviceProvider, "Debug"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE;

            rs.textureStage[0].colorOp = TOP_MODULATE;
            rs.textureStage[0].alphaOp = TOP_MODULATE;

            this->createRenderStageGroup( CONST_STRING_LOCAL(m_serviceProvider, "ParticleIntensive"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            rs.textureStage[0].colorOp = TOP_MODULATE;
            rs.textureStage[0].alphaOp = TOP_MODULATE;

            this->createRenderStageGroup( CONST_STRING_LOCAL(m_serviceProvider, "ParticleBlend"), rs );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderMaterialManager::finalize()
    {
		for( TMapRenderStage::iterator
			it = m_stages.begin(),
			it_end = m_stages.end();
		it != it_end;
		++it )
		{
			RenderStageGroup * stage = m_stages.get_value( it );

			stage->destroy();
		}

		m_stages.clear();
		m_materials.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderMaterialManager::loadMaterials( const ConstString& _pakName, const FilePath& _fileName )
    {
        (void)_pakName;
        (void)_fileName;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr RenderMaterialManager::getMaterial( const ConstString & _stageName
		, bool _wrapU
		, bool _wrapV
		, EPrimitiveType _primitiveType
		, size_t _textureCount
		, const RenderTextureInterfacePtr * _textures )
	{
		const RenderStageGroup * stageGroup;
		if( m_stages.has( _stageName, &stageGroup ) == false )
		{
			return nullptr;
		}

		const RenderStage * stage = &stageGroup->stage[ (_wrapU ? 1 : 0) + (_wrapV ? 2 : 0) ];

		RenderMaterialPtr material = m_materials.create();

		size_t id = ++m_materialEnumerator;
		material->initialize( id, _primitiveType, _textureCount, _textures, stage );
		
		RenderMaterial * insert_material;
		if( m_materials.insert_exist( material.get(), &insert_material ) == false )
		{
			return insert_material;
		}

		return material;
	}
    //////////////////////////////////////////////////////////////////////////
    bool RenderMaterialManager::createRenderStageGroup( const ConstString & _name, const RenderStage & _stage )
    {
        if( m_stages.exist( _name ) == true )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::createRenderStage: RenderStage '%s' is already created!"
                , _name.c_str()
                );

            return false;
        }
		
		RenderStageGroup * stageGroup = m_factoryStage.createObjectT();

		stageGroup->stage[0] = _stage;
		stageGroup->stage[1] = _stage;
		stageGroup->stage[2] = _stage;
		stageGroup->stage[3] = _stage;

		for( size_t i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			stageGroup->stage[0].textureStage[i].addressU = TAM_CLAMP;
			stageGroup->stage[0].textureStage[i].addressV = TAM_CLAMP;

			stageGroup->stage[1].textureStage[i].addressU = TAM_WRAP;
			stageGroup->stage[1].textureStage[i].addressV = TAM_CLAMP;

			stageGroup->stage[2].textureStage[i].addressU = TAM_CLAMP;
			stageGroup->stage[2].textureStage[i].addressV = TAM_WRAP;

			stageGroup->stage[3].textureStage[i].addressU = TAM_WRAP;
			stageGroup->stage[3].textureStage[i].addressV = TAM_WRAP;
		}

		m_stages.insert( _name, stageGroup );

        return true;
    }
}