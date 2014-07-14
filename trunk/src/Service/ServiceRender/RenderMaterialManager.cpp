#   include "RenderMaterialManager.h"

#   include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

#	include "stdex/hash.h"

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
		m_factoryMaterial.setMethodListener( this, &RenderMaterialManager::onRenderMaterialDestroy_ );

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

		for( size_t i = 0; i != MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE; ++i )
		{
			TVectorRenderMaterial & material = m_materials[i];

			material.clear();
		}
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderMaterialManager::loadMaterials( const ConstString& _pakName, const FilePath& _fileName )
    {
        (void)_pakName;
        (void)_fileName;

        return false;
    }
	//////////////////////////////////////////////////////////////////////////
	static bool s_equalMaterial( const RenderMaterial * _material
		, EPrimitiveType _primitiveType
		, size_t _textureCount 
		, const RenderTextureInterfacePtr * _textures
		, const RenderStage * _stage
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

		for( size_t i = 0; i != _textureCount; ++i )
		{
			if( _material->getTexture(i) != _textures[i] )
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
	static uint32_t s_makeHash( EPrimitiveType _primitiveType, size_t _textureCount, const RenderTextureInterfacePtr * _textures, const RenderStage * _stage )
	{
		uint32_t hash = stdex::hash_base;

		hash = stdex::hash_binary( hash, _primitiveType );
		hash = stdex::hash_binary( hash, _textureCount );

		for( size_t i = 0; i != _textureCount; ++i )
		{
			hash = stdex::hash_binary( hash, _textures[i].get() );
		}

		hash = stdex::hash_binary( hash, _stage );

		return hash;
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

		//uint32_t material_hash = s_makeHash( _primitiveType, _textureCount, _textures, stage );

		//uint32_t material_table_index = material_hash / 33554433U;


		uint32_t material_hash = _textureCount ? _textures[0]->getId() : 0U;
		uint32_t material_table_index = material_hash % MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE;

		TVectorRenderMaterial & materials = m_materials[material_table_index];

		for( TVectorRenderMaterial::iterator
			it = materials.begin(),
			it_end = materials.end();
		it != it_end;
		++it )
		{
			RenderMaterial * material = *it;

			if( material->getHash() != material_hash )
			{
				continue;
			}
			
			if( s_equalMaterial( material, _primitiveType, _textureCount, _textures, stage ) == false )
			{
				continue;
			}

			return material;
		}

		RenderMaterial * material = m_factoryMaterial.createObjectT();

		size_t id = ++m_materialEnumerator;
		material->initialize( id, material_hash, _primitiveType, _textureCount, _textures, stage );
		
		materials.push_back( material );

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMaterialManager::onRenderMaterialDestroy_( RenderMaterial * _material )
	{
		if( _material == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderMaterialManager::releaseMaterial material is nullptr"
				);

			return;
		}

		uint32_t material_hash = _material->getHash();

		//uint32_t material_table_index = material_hash % MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE;
		uint32_t material_table_index = material_hash % MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE;

		TVectorRenderMaterial & materials = m_materials[material_table_index];

		for( TVectorRenderMaterial::iterator
			it = materials.begin(),
			it_end = materials.end();
		it != it_end;
		++it )
		{
			RenderMaterial * material = *it;

			if( material->getHash() != material_hash )
			{
				continue;
			}
			
			EPrimitiveType primitiveType = _material->getPrimitiveType();
			size_t textureCount = _material->getTextureCount();
			const RenderTextureInterfacePtr * textures = _material->getTextures();
			const RenderStage * stage = _material->getStage();

			if( s_equalMaterial( material, primitiveType, textureCount, textures, stage ) == false )
			{
				continue;
			}
			
			*it = materials.back();
			materials.pop_back();
			
			return;
		}
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