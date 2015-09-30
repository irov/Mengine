#   include "RenderMaterialManager.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/ConfigInterface.h"
#   include "Interface/LoaderInterface.h"

#   include "Logger/Logger.h"

#	include "Core/MemoryCacheBuffer.h"

#	include <Metacode/Metacode.h>

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
		, m_stageCount(0)
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
		m_defaultStages[EM_DEBUG] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Debug" );

		m_defaultStages[EM_TEXTURE_SOLID] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Solid" );

		m_defaultStages[EM_TEXTURE_BLEND] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Blend" );
		m_defaultStages[EM_TEXTURE_BLEND_WC] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Blend_WC" );
		m_defaultStages[EM_TEXTURE_BLEND_WW] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Blend_WW" );
		m_defaultStages[EM_TEXTURE_BLEND_CW] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Blend_CW" );
		m_defaultStages[EM_TEXTURE_INTENSIVE] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Intensive" );
		m_defaultStages[EM_TEXTURE_MULTIPLY] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Multiply" );
		m_defaultStages[EM_TEXTURE_SCREEN] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Screen" );

		m_defaultStages[EM_TEXTURE_BLEND_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Blend_OnlyColor" );
		m_defaultStages[EM_TEXTURE_INTENSIVE_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Intensive_OnlyColor" );
		m_defaultStages[EM_TEXTURE_MULTIPLY_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Multiply_OnlyColor" );
		m_defaultStages[EM_TEXTURE_SCREEN_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Screen_OnlyColor" );

		m_defaultStages[EM_TEXTURE_BLEND_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Blend_ExternalAlpha" );
		m_defaultStages[EM_TEXTURE_BLEND_EXTERNAL_ALPHA_WC] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Blend_ExternalAlpha_WC" );
		m_defaultStages[EM_TEXTURE_BLEND_EXTERNAL_ALPHA_WW] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Blend_ExternalAlpha_WW" );
		m_defaultStages[EM_TEXTURE_BLEND_EXTERNAL_ALPHA_CW] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Blend_ExternalAlpha_CW" );
		m_defaultStages[EM_TEXTURE_INTENSIVE_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Intensive_ExternalAlpha" );
		m_defaultStages[EM_TEXTURE_MULTIPLY_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Multiply_ExternalAlpha" );
		m_defaultStages[EM_TEXTURE_SCREEN_EXTERNAL_ALPHA] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Screen_ExternalAlpha" );

		m_defaultStages[EM_TEXTURE_BLEND_EXTERNAL_ALPHA_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Blend_ExternalAlpha_OnlyColor" );
		m_defaultStages[EM_TEXTURE_INTENSIVE_EXTERNAL_ALPHA_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Intensive_ExternalAlpha_OnlyColor" );
		m_defaultStages[EM_TEXTURE_MULTIPLY_EXTERNAL_ALPHA_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Multiply_ExternalAlpha_OnlyColor" );
		m_defaultStages[EM_TEXTURE_SCREEN_EXTERNAL_ALPHA_ONLYCOLOR] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Texture_Screen_ExternalAlpha_OnlyColor" );

		m_defaultStages[EM_COLOR_SOLID] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Color_Solid" );
		m_defaultStages[EM_COLOR_BLEND] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Color_Blend" );
		m_defaultStages[EM_COLOR_INTENSIVE] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Color_Intensive" );
		m_defaultStages[EM_COLOR_MULTIPLY] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Color_Multiply" );
		m_defaultStages[EM_COLOR_SCREEN] = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Color_Screen" );

		m_factoryMaterial.setMethodListener( this, &RenderMaterialManager::onRenderMaterialDestroy_ );

		return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderMaterialManager::finalize()
    {
		for( uint32_t i = 0; i != MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE; ++i )
		{
			TVectorRenderMaterial & material = m_materials[i];

			material.clear();
		}
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderMaterialManager::loadMaterials( const ConstString& _pakName, const FilePath& _fileName )
    {
		Metacode::Meta_DataBlock datablock;

		bool exist = false;
		if( LOADER_SERVICE( m_serviceProvider )
			->load( _pakName, _fileName, &datablock, exist ) == false )
		{
			if( exist == false )
			{
				LOGGER_ERROR( m_serviceProvider )("RenderMaterialManager::loadMaterials: materials '%s:%s' not found"
					, _pakName.c_str()
					, _fileName.c_str()
					);
			}
			else
			{
				LOGGER_ERROR( m_serviceProvider )("RenderMaterialManager::loadMaterials: Invalid parse materials '%s:%s'"
					, _pakName.c_str()
					, _fileName.c_str()
					);
			}

			return false;
		}

		const ConstString & renderPlatformName = RENDER_SYSTEM( m_serviceProvider )
			->getRenderPlatformName();

		const Metacode::Meta_DataBlock::TVectorMeta_FragmentShader & includes_FragmentShader = datablock.get_IncludesFragmentShader();

		for( Metacode::Meta_DataBlock::TVectorMeta_FragmentShader::const_iterator
			it = includes_FragmentShader.begin(),
			it_end = includes_FragmentShader.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_DataBlock::Meta_FragmentShader & meta_FragmentShader = *it;

			const ConstString & name = meta_FragmentShader.get_Name();
			const ConstString & platform = meta_FragmentShader.get_Platform();

			if( platform != renderPlatformName )
			{
				continue;
			}

			const ConstString & filePath = meta_FragmentShader.get_File_Path();

			bool isCompile = false;
			meta_FragmentShader.get_File_Compile( isCompile );

			if( this->loadFragmentShader_( name, _pakName, filePath, isCompile ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("RenderMaterialManager::loadMaterials material %s:%s invalid load fragment shader %s compile %d"
					, _pakName.c_str()
					, _fileName.c_str()
					, filePath.c_str()
					, isCompile
					);

				return false;
			}
		}

		const Metacode::Meta_DataBlock::TVectorMeta_VertexShader & includes_VertexShader = datablock.get_IncludesVertexShader();

		for( Metacode::Meta_DataBlock::TVectorMeta_VertexShader::const_iterator
			it = includes_VertexShader.begin(),
			it_end = includes_VertexShader.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_DataBlock::Meta_VertexShader & meta_VertexShader = *it;

			const ConstString & name = meta_VertexShader.get_Name();
			const ConstString & platform = meta_VertexShader.get_Platform();

			if( platform != renderPlatformName )
			{
				continue;
			}

			const ConstString & filePath = meta_VertexShader.get_File_Path();

			bool isCompile = false;
			meta_VertexShader.get_File_Compile( isCompile );

			if( this->loadVertexShader_( name, _pakName, filePath, isCompile ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("RenderMaterialManager::loadMaterials material %s:%s invalid load vertex shader %s compile %d"
					, _pakName.c_str()
					, _fileName.c_str()
					, filePath.c_str()
					, isCompile
					);

				return false;
			}
		}

		const Metacode::Meta_DataBlock::TVectorMeta_Program & includes_Program = datablock.get_IncludesProgram();

		for( Metacode::Meta_DataBlock::TVectorMeta_Program::const_iterator
			it = includes_Program.begin(),
			it_end = includes_Program.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_DataBlock::Meta_Program & meta_Program = *it;

			const ConstString & name = meta_Program.get_Name();
			const ConstString & platform = meta_Program.get_Platform();

			if( platform != renderPlatformName )
			{
				continue;
			}

			const ConstString & vertexShaderName = meta_Program.get_VertexShader_Name();
			const ConstString & fragmentShaderName = meta_Program.get_FragmentShader_Name();
			uint32_t samplerCount = meta_Program.get_Sampler_Count();

			const RenderShaderInterfacePtr & vertexShader = this->getVertexShader_( vertexShaderName );
			
			if( vertexShader == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("RenderMaterialManager::loadMaterials material %s:%s program %s not found vertex shader %s"
					, _pakName.c_str()
					, _fileName.c_str()
					, name.c_str()
					, vertexShaderName.c_str()
					);

				return false;
			}

			const RenderShaderInterfacePtr & fragmentShader = this->getFragmentShader_( fragmentShaderName );
				
			if( fragmentShader == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("RenderMaterialManager::loadMaterials material %s:%s program %s not found fragment shader %s"
					, _pakName.c_str()
					, _fileName.c_str()
					, name.c_str()
					, fragmentShaderName.c_str()
					);

				return false;
			}

			RenderProgramInterfacePtr program = RENDER_SYSTEM( m_serviceProvider )
				->createProgram( name, vertexShader, fragmentShader, samplerCount );

			if( program == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("RenderMaterialManager::loadMaterials material %s:%s invalid create program vertex %s fragment %s"
					, _pakName.c_str()
					, _fileName.c_str()
					, vertexShaderName.c_str()
					, fragmentShaderName.c_str()
					);

				return false;
			}

			m_programs.insert( std::make_pair( name, program ) );
		}

		const Metacode::Meta_DataBlock::TVectorMeta_Material & includes_Material = datablock.get_IncludesMaterial();

		for( Metacode::Meta_DataBlock::TVectorMeta_Material::const_iterator
			it = includes_Material.begin(),
			it_end = includes_Material.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_DataBlock::Meta_Material & meta_Material = *it;

			const Menge::ConstString & name = meta_Material.get_Name();

			bool is_debug = false;
			meta_Material.get_Debug( is_debug );

			RenderStage stage;
			meta_Material.get_AlphaBlend_Enable( stage.alphaBlendEnable );
			meta_Material.get_BlendFactor_Source( stage.blendSrc );
			meta_Material.get_BlendFactor_Dest( stage.blendDst );

			ConstString programName;
			if( meta_Material.get_Program_Name( programName ) == true )
			{
				const RenderProgramInterfacePtr & program = this->getProgram_( programName );

				if( program == nullptr )
				{
					//LOGGER_ERROR( m_serviceProvider )("RenderMaterialManager::loadMaterials material %s:%s invalid get program %s"
					//	, _pakName.c_str()
					//	, _fileName.c_str()
					//	, programName.c_str()
					//	);

					//return false;
				}

				stage.program = program;
			}

			const Metacode::Meta_DataBlock::Meta_Material::TVectorMeta_TextureStages & include_TextureStages = meta_Material.get_IncludesTextureStages();

			for( Metacode::Meta_DataBlock::Meta_Material::TVectorMeta_TextureStages::const_iterator
				it = include_TextureStages.begin(),
				it_end = include_TextureStages.end();
			it != it_end;
			++it )
			{
				const Metacode::Meta_DataBlock::Meta_Material::Meta_TextureStages & meta_TextureStages = *it;

				uint32_t index = meta_TextureStages.get_Stage();

				RenderTextureStage & textureStage = stage.textureStage[index];

				meta_TextureStages.get_AddressMode_U( textureStage.addressU );
				meta_TextureStages.get_AddressMode_V( textureStage.addressV );

				textureStage.colorOp = meta_TextureStages.get_Color_Operator();
				meta_TextureStages.get_Color_Arg1( textureStage.colorArg1 );
				meta_TextureStages.get_Color_Arg2( textureStage.colorArg2 );

				textureStage.alphaOp = meta_TextureStages.get_Alpha_Operator();
				meta_TextureStages.get_Alpha_Arg1( textureStage.alphaArg1 );
				meta_TextureStages.get_Alpha_Arg2( textureStage.alphaArg2 );

				meta_TextureStages.get_TextureCoord_Index( textureStage.texCoordIndex );
			}

			if( this->createRenderStageGroup( name, stage ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("RenderMaterialManager::loadMaterials material %s:%s invalid create stage group %s"
					, _pakName.c_str()
					, _fileName.c_str()
					, name.c_str()
					);

				return false;
			}

			if( is_debug == true )
			{
				RenderMaterialInterfacePtr debugMaterial = 
					this->getMaterial( name, PT_LINELIST, 0, nullptr );

				this->setDebugMaterial( debugMaterial );
			}
		}

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	static bool s_equalMaterial( const RenderMaterial * _material
		, EPrimitiveType _primitiveType
		, uint32_t _textureCount 
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
	const ConstString & RenderMaterialManager::getMaterialName( EMaterial _materialId ) const
	{
		if( _materialId >= EM_MATERIAL_COUNT )
		{
			return ConstString::none();
		}

		const ConstString & materialName = m_defaultStages[_materialId];

		return materialName;
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

		if( _src.colorOp != _dst.colorOp )
		{
			return false;
		}

		if( _src.colorArg1 != _dst.colorArg1 )
		{
			return false;
		}

		if( _src.colorArg2 != _dst.colorArg2 )
		{
			return false;
		}

		if( _src.alphaOp != _dst.alphaOp )
		{
			return false;
		}

		if( _src.alphaArg1 != _dst.alphaArg1 )
		{
			return false;
		}

		if( _src.alphaArg2 != _dst.alphaArg2 )
		{
			return false;
		}

		if( _src.texCoordIndex != _dst.texCoordIndex )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_equalRenderStage( const RenderStage & _src, const RenderStage & _dst )
	{
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

		if( _src.program != _dst.program )
		{
			return false;
		}

		for( uint32_t i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			const RenderTextureStage & src_textureStage = _src.textureStage[i];
			const RenderTextureStage & dst_textureStage = _dst.textureStage[i];

			if( s_equalTextureStage( src_textureStage, dst_textureStage ) == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderStage * RenderMaterialManager::cacheStage( const RenderStage & _other )
	{
		for( uint32_t it = 0; it != m_stageCount; ++it )
		{
			const RenderStage & self = m_stages[it];

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

		const RenderStage & cache_other = m_stages[m_stageCount];

		m_stageCount++;

		return &cache_other;
	}
    //////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr RenderMaterialManager::getMaterial( const ConstString & _materialName
		, EPrimitiveType _primitiveType
		, uint32_t _textureCount
		, const RenderTextureInterfacePtr * _textures )
	{
		TMapRenderStage::const_iterator it_found = m_stageIndexer.find( _materialName );

		if( it_found == m_stageIndexer.end() )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderMaterialManager::getMaterial stage %s not found"
				, _materialName.c_str()
				);

			return nullptr;
		}

		const RenderStage * stage = it_found->second;

		for( uint32_t i = 0; i != _textureCount; ++i )
		{
			if( _textures[i] == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("RenderMaterialManager::getMaterial stage %s invalid setup texture %d"
					, _materialName.c_str()
					, i
					);

				return nullptr;
			}
		}

		RenderMaterialInterfacePtr material = this->getMaterial2( stage, _primitiveType, _textureCount, _textures );

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr RenderMaterialManager::getMaterial2( const RenderStage * _stage
		, EPrimitiveType _primitiveType
		, uint32_t _textureCount
		, const RenderTextureInterfacePtr * _textures )
	{
		uint32_t material_hash = this->makeMaterialHash( _textureCount, _textures );

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
			
			if( s_equalMaterial( material, _primitiveType, _textureCount, _textures, _stage ) == false )
			{
				continue;
			}

			return material;
		}

		RenderMaterial * material = m_factoryMaterial.createObjectT();

		uint32_t id = this->makeMaterialIndex_();
		material->initialize( id, material_hash, _primitiveType, _textureCount, _textures, _stage );
		
		materials.push_back( material );

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMaterialManager::setDebugMaterial( const RenderMaterialInterfacePtr & _debugMaterial )
	{
		m_debugMaterial = _debugMaterial;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderMaterialInterfacePtr & RenderMaterialManager::getDebugMaterial() const
	{
		return m_debugMaterial;
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

		uint32_t material_table_index = material_hash % MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE;

		TVectorRenderMaterial & materials = m_materials[material_table_index];

		for( TVectorRenderMaterial::iterator
			it = materials.begin(),
			it_end = materials.end();
		it != it_end;
		++it )
		{
			RenderMaterial * material = *it;

			if( material != _material )
			{
				continue;
			}
			
			*it = materials.back();
			materials.pop_back();
			
			break;
		}

		uint32_t materialId = _material->getId();
		m_materialIndexer.push_back( materialId );
	}
    //////////////////////////////////////////////////////////////////////////
    bool RenderMaterialManager::createRenderStageGroup( const ConstString & _name, const RenderStage & _stage )
    {
		TMapRenderStage::const_iterator it_found = m_stageIndexer.find( _name );

		if( it_found != m_stageIndexer.end() )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::createRenderStage: RenderStage '%s' is already created"
                , _name.c_str()
                );

            return false;
        }

		const RenderStage * cache_stage = this->cacheStage( _stage );

		if( cache_stage == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderEngine::createRenderStage: RenderStage '%s' invalid cache"
				, _name.c_str()
				);

			return false;
		}
		
		m_stageIndexer.insert( std::make_pair( _name, cache_stage ) );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t RenderMaterialManager::makeMaterialIndex_()
	{
		if( m_materialIndexer.empty() == false )
		{
			uint32_t id = m_materialIndexer.back();
			m_materialIndexer.pop_back();

			return id;
		}

		return ++m_materialEnumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t RenderMaterialManager::makeMaterialHash( uint32_t _textureCount, const RenderTextureInterfacePtr * _textures ) const
	{
		uint32_t material_hash = 0U;
		for( uint32_t i = 0; i != _textureCount; ++i )
		{
			uint32_t texture_id = _textures[i]->getId();

			material_hash += texture_id;
		}

		return material_hash;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderMaterialManager::loadFragmentShader_( const ConstString & _name, const ConstString & _pakName, const ConstString & _filePath, bool isCompile )
	{ 
		MemoryCacheBufferInterfacePtr data_cache = Helper::createMemoryFile( m_serviceProvider, _pakName, _filePath, false, "loadFragmentShader" );
		
		if( data_cache == nullptr )
		{
			return false;
		}
				
		const void * buffer = data_cache->getMemory();
		size_t size = data_cache->getSize();

		RenderShaderInterfacePtr shader = RENDER_SYSTEM( m_serviceProvider )
			->createFragmentShader( _name, buffer, size, isCompile );

		if( shader == nullptr )
		{
			return false;
		}

		m_fragmentShaders.insert( std::make_pair( _name, shader ) );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderMaterialManager::loadVertexShader_( const ConstString & _name, const ConstString & _pakName, const ConstString & _filePath, bool isCompile )
	{ 
		MemoryCacheBufferInterfacePtr data_cache = Helper::createMemoryFile( m_serviceProvider, _pakName, _filePath, false, "loadVertexShader" );

		if( data_cache == nullptr )
		{
			return false;
		}

		const void * buffer = data_cache->getMemory();
		size_t size = data_cache->getSize();

		RenderShaderInterfacePtr shader = RENDER_SYSTEM( m_serviceProvider )
			->createVertexShader( _name, buffer, size, isCompile );

		if( shader == nullptr )
		{
			return false;
		}

		m_vertexShaders.insert( std::make_pair( _name, shader ) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderShaderInterfacePtr & RenderMaterialManager::getVertexShader_( const ConstString & _name ) const
	{
		TMapRenderShaders::const_iterator it_found = m_vertexShaders.find( _name );

		if( it_found == m_vertexShaders.end() )
		{
			return RenderShaderInterfacePtr::none();
		}

		const RenderShaderInterfacePtr & shader = it_found->second;

		return shader;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderShaderInterfacePtr & RenderMaterialManager::getFragmentShader_( const ConstString & _name ) const
	{
		TMapRenderShaders::const_iterator it_found = m_fragmentShaders.find( _name );

		if( it_found == m_fragmentShaders.end() )
		{
			return RenderShaderInterfacePtr::none();
		}

		const RenderShaderInterfacePtr & shader = it_found->second;

		return shader;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderProgramInterfacePtr & RenderMaterialManager::getProgram_( const ConstString & _name ) const
	{
		TMapRenderPrograms::const_iterator it_found = m_programs.find( _name );

		if( it_found == m_programs.end() )
		{
			return RenderProgramInterfacePtr::none();
		}

		const RenderProgramInterfacePtr & program = it_found->second;

		return program;
	}
}