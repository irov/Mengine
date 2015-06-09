#   include "RenderMaterialManager.h"

#   include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

#	include "stdex/hash.h"

#	include "s3e.h"

#	include "Shaders/DefaultVS.h"

#	include "Shaders/ExternalAlphaFS.h"

#	include "Shaders/ExternalAlphaOnlyColorFS.h"

#	include "Shaders/BlendFS.h"

#	include "Shaders/AccumulatorFS.h"

#	include "Shaders/DebugFS.h"

#	include "Shaders/OnlyColorFS.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderMaterialManager, Menge::RenderMaterialServiceInterface, Menge::RenderMaterialManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	#define VERTEX_ARRAY			0
	#define COLOR_ARRAY				1
	#define UV0_ARRAY				2
	#define UV1_ARRAY				3

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
	GLuint createShader(GLenum type, const char* pSource)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &pSource, NULL);
		glCompileShader(shader);
		return shader;
	}

	GLuint RenderMaterialManager::shaderProgram(const char * _vsSrc, const char * _fsSrc, int & transformLocation)
	{
		const char* vertexSrc = "attribute vec4 position; uniform highp mat4 mat; varying mediump vec2 pos; void main() { gl_Position = position * mat; pos = position.xy; }";
		const char* fragmentSrc = "void main() { gl_FragColor = vec4(1, 0, 0, 0.5); }";
		//const char* fragmentSrc = "varying mediump vec2 pos; uniform mediump float phase; void main() { gl_FragColor = vec4(1, 1, 1, 1) * sin((pos.x * pos.x + pos.y * pos.y) * 40.0 + phase); }";
		//_vsSrc = vertexSrc;
		_fsSrc = externalAlphaFS;

		GLuint program = glCreateProgram();
		GLuint vertexShader = 0;
		GLuint fragmentShader = 0;
		if ((s3eDeviceGetInt(S3E_DEVICE_OS) != S3E_OS_ID_WS8 &&
			s3eDeviceGetInt(S3E_DEVICE_OS) != S3E_OS_ID_WS81 &&
			s3eDeviceGetInt(S3E_DEVICE_OS) != S3E_OS_ID_WP81
			) ||
			s3eDeviceGetInt(S3E_DEVICE_DX_FEATURE_LEVEL) < 93
			)
		{
			vertexShader = createShader(GL_VERTEX_SHADER, _vsSrc);
			fragmentShader = createShader(GL_FRAGMENT_SHADER, _fsSrc);
			// This is needed to compile shaders for Windows Store 8/8.1 and Windows Phone 8.1 using the Win32 Marmalade Simulator.
			// For more information look at README.IwGLES2.txt.
			if (IwGetCompileShadersPlatformType() == IW_CS_OS_ID_WS8 ||
				IwGetCompileShadersPlatformType() == IW_CS_OS_ID_WS81 ||
				IwGetCompileShadersPlatformType() == IW_CS_OS_ID_WP81
				)
			{
				s3eRegisterShader(_vsSrc, IW_GL_ST_VERTEX, IW_DX_FL_9_3);
				s3eRegisterShader(_fsSrc, IW_GL_ST_PIXEL, IW_DX_FL_9_3);
			}
		}
		else
		{
			vertexShader = createShader(GL_VERTEX_SHADER, _vsSrc);
			fragmentShader = createShader(GL_FRAGMENT_SHADER, _fsSrc);
		}
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glBindAttribLocation(program, VERTEX_ARRAY, "inVert");
		glBindAttribLocation(program, COLOR_ARRAY, "inCol");
		glBindAttribLocation(program, UV0_ARRAY, "inUV0");
		glBindAttribLocation(program, UV1_ARRAY, "inUV1");

		glLinkProgram(program);

		GLint linked;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (linked == GL_FALSE)
		{
			LOGGER_ERROR(m_serviceProvider)("RenderMaterialManager::shaderProgram - shader linking error");
		}
		glUseProgram(program);
		transformLocation = glGetUniformLocation(program, "mvpMat");

		return program;
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

			rs.shaderProgram = shaderProgram(defaultVS, blendFS, rs.transformLocation);

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            rs.textureStage[0].alphaOp = TOP_MODULATE;
            rs.textureStage[0].colorOp = TOP_MODULATE;

            this->createRenderStageGroup( STRINGIZE_STRING_LOCAL(m_serviceProvider, "Blend"), rs );
		}

        {
            RenderStage rs;

            rs.alphaBlendEnable = false;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

			rs.shaderProgram = shaderProgram(defaultVS, blendFS, rs.transformLocation);

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            rs.textureStage[0].alphaOp = TOP_MODULATE;
            rs.textureStage[0].colorOp = TOP_MODULATE;

            this->createRenderStageGroup( STRINGIZE_STRING_LOCAL(m_serviceProvider, "SolidSprite"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

			rs.shaderProgram = shaderProgram(defaultVS, accumulatorFS, rs.transformLocation);

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            rs.textureStage[0].alphaOp = TOP_MODULATE;
            rs.textureStage[0].colorOp = TOP_SELECTARG1;

            this->createRenderStageGroup( STRINGIZE_STRING_LOCAL(m_serviceProvider, "Accumulator"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;
			
			rs.shaderProgram = shaderProgram(defaultVS, externalAlphaFS, rs.transformLocation);

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
			
            this->createRenderStageGroup( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ExternalAlpha"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE;

			rs.shaderProgram = shaderProgram(defaultVS, externalAlphaFS, rs.transformLocation);

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

            this->createRenderStageGroup( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ExternalAlphaIntensive"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

			rs.shaderProgram = shaderProgram(defaultVS, externalAlphaOnlyColorFS, rs.transformLocation);

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

            this->createRenderStageGroup( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ExternalAlpha_OnlyColor"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE;

			rs.shaderProgram = shaderProgram(defaultVS, externalAlphaOnlyColorFS, rs.transformLocation);

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

            this->createRenderStageGroup( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ExternalAlphaIntensive_OnlyColor"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

			rs.shaderProgram = shaderProgram(defaultVS, onlyColorFS, rs.transformLocation);

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            rs.textureStage[0].alphaOp = TOP_MODULATE;
            rs.textureStage[0].colorOp = TOP_SELECTARG2;

            this->createRenderStageGroup( STRINGIZE_STRING_LOCAL(m_serviceProvider, "OnlyColor"), rs );
        }

		{
			RenderStage rs;

			rs.alphaBlendEnable = true;
			rs.alphaTestEnable = false;
			rs.depthBufferWriteEnable = false;

			rs.shaderProgram = shaderProgram(defaultVS, onlyColorFS, rs.transformLocation);

			rs.blendSrc = BF_SOURCE_ALPHA;
			rs.blendDst = BF_ONE;

			rs.textureStage[0].alphaOp = TOP_MODULATE;
			rs.textureStage[0].colorOp = TOP_SELECTARG2;

			this->createRenderStageGroup( STRINGIZE_STRING_LOCAL(m_serviceProvider, "Intensive_OnlyColor"), rs );
		}

        {
            RenderStage rs;

            rs.alphaBlendEnable = false;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

			rs.shaderProgram = shaderProgram(defaultVS, debugFS, rs.transformLocation);

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

            rs.textureStage[0].colorOp = TOP_SELECTARG2;
            rs.textureStage[0].alphaOp = TOP_SELECTARG2;

            this->createRenderStageGroup( STRINGIZE_STRING_LOCAL(m_serviceProvider, "Debug"), rs );
        }

        {
            RenderStage rs;

            rs.alphaBlendEnable = true;
            rs.alphaTestEnable = false;
            rs.depthBufferWriteEnable = false;

			rs.shaderProgram = shaderProgram(defaultVS, blendFS, rs.transformLocation);

            rs.blendSrc = BF_SOURCE_ALPHA;
            rs.blendDst = BF_ONE;

            rs.textureStage[0].colorOp = TOP_MODULATE;
            rs.textureStage[0].alphaOp = TOP_MODULATE;

            this->createRenderStageGroup( STRINGIZE_STRING_LOCAL(m_serviceProvider, "Add"), rs );
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

		for( uint32_t i = 0; i != MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE; ++i )
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
	RenderMaterialInterfacePtr RenderMaterialManager::getMaterial( const ConstString & _stageName
		, bool _wrapU
		, bool _wrapV
		, EPrimitiveType _primitiveType
		, uint32_t _textureCount
		, const RenderTextureInterfacePtr * _textures )
	{
		const RenderStageGroup * stageGroup;
		if( m_stages.has( _stageName, &stageGroup ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderMaterialManager::getMaterial stage %s not found"
				, _stageName.c_str()
				);

			return nullptr;
		}

		for( uint32_t i = 0; i != _textureCount; ++i )
		{
			if( _textures[i] == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("RenderMaterialManager::getMaterial stage %s invalid setup texture %d"
					, _stageName.c_str()
					, i
					);

				return nullptr;
			}
		}

		uint32_t stageWrapId = (_wrapU ? 1 : 0) + (_wrapV ? 2 : 0);

		const RenderStage * stage = &stageGroup->stage[stageWrapId];

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

		uint32_t id = this->makeMaterialIndex_();
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
			uint32_t textureCount = _material->getTextureCount();
			const RenderTextureInterfacePtr * textures = _material->getTextures();
			const RenderStage * stage = _material->getStage();

			if( s_equalMaterial( material, primitiveType, textureCount, textures, stage ) == false )
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

		for( uint32_t i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
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
}