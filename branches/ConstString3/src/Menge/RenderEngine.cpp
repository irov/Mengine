#	include "RenderEngine.h"

#	include "FileEngine.h"

#	include "LogEngine.h"

#	include "Application.h"
#	include "Game.h"

#	include "CodecEngine.h"
#	include "Interface/ImageCodecInterface.h"


//#	include "ResourceTexture.h"
#	include "ResourceImage.h"
#	include "RenderMaterial.h"

#	include "Camera.h"

#	include "RenderTexture.h"
#	include "RenderSubTexture.h"

#	include "Megatextures.h"

#	include "Vertex.h"

#	include "Core/PixelFormat.h"

#	include "Consts.h"

#	include <algorithm>
#	include <map>
#	include <ctime>

#	include  "../Plugin/MengeImageCodecPlugin/ImageDecoderCombinerRGBAndAlpha.h"

namespace Menge
{
	struct RenderObject
	{
		const RenderMaterial * material;

		size_t textureStages;
		const RenderTextureInterface * textures[MENGE_MAX_TEXTURE_STAGES];

		const mt::mat4f * matrixUV[MENGE_MAX_TEXTURE_STAGES];

		ELogicPrimitiveType logicPrimitiveType;
		EPrimitiveType primitiveType;

		const Vertex2D * vertexData;
		size_t verticesNum;
		size_t minIndex;
		size_t startIndex;

		size_t dipIndiciesNum;
		size_t dipVerticesNum;

		IBHandle ibHandle;
		size_t baseVertexIndex;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef std::vector<RenderObject> TVectorRenderObject;
	//////////////////////////////////////////////////////////////////////////
	struct RenderPass
	{
		TVectorRenderObject renderObjects;

		const RenderCameraInterface * camera;
		//Viewport viewport;
		//mt::mat4f viewMatrix;
		//mt::mat4f projectionMatrix;
		//mt::mat4f wm;
	};
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine()
		: m_interface(NULL)
		, m_windowCreated(false)
		, m_vsync(false)
		, m_layer3D(false)
		, m_vbHandle2D(0)
		, m_ibHandle2D(0)
		, m_currentVBHandle(0)
		, m_currentIBHandle(0)
		, m_currentBaseVertexIndex(0)
		, m_currentTextureStages(0)
		, m_currentPass(NULL)
		, m_currentRenderCamera(NULL)
		, m_nullTexture(NULL)
		, m_currentVertexDeclaration(0)
		, m_maxPrimitiveVertices2D(0)
		, m_maxIndexCount(0)
		, m_depthBufferWriteEnable(false)
		, m_alphaBlendEnable(false)
		, m_alphaTestEnable(false)
		, m_textureFiltering(true)
		, m_idEnumerator(0)
		, m_supportA8(false)
		, m_dipCount(0)
		, m_renderScale(0.f, 0.f)
		, m_renderOffset(0.f, 0.f)
		, m_megatextures(NULL)
		, m_vbPos(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::~RenderEngine()
	{
		//for( TVectorRenderPass::iterator
		//	it = m_passes.begin(),
		//	it_end = m_passes.end();
		//it != it_end;
		//++it )
		//{
		//	m_poolRenderPass.release( *it );
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::initialize( size_t _maxQuadCount )
	{
		bool result = initInterfaceSystem( &m_interface );
		if( ( result == false ) || ( m_interface == NULL ) )
		{
			return false;
		}

		m_maxQuadCount = _maxQuadCount;

		this->setRenderSystemDefaults_( m_maxQuadCount );

		ServiceProviderInterface * serviceProvider = Application::get()
			->getServiceProvider();


		if( m_interface->initialize( serviceProvider, this ) == false )
		{
			return false;
		}
		
		{
			RenderMaterial mt;

			mt.alphaBlendEnable = true;
			mt.alphaTestEnable = true;
			mt.depthBufferWriteEnable = false;

			mt.blendSrc = BF_SOURCE_ALPHA;
			mt.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

			mt.textureStage[0].alphaOp = TOP_MODULATE;
			mt.textureStage[0].colorOp = TOP_MODULATE;

			this->createMaterialGroup( CONST_STRING(BlendSprite), mt );
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

			this->createMaterialGroup( CONST_STRING(SolidSprite), mt );
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

			this->createMaterialGroup( CONST_STRING(Accumulator), mt );
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

			this->createMaterialGroup( CONST_STRING(ExternalAlpha), mt );
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

			this->createMaterialGroup( CONST_STRING(OnlyColor), mt );
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

			this->createMaterialGroup( CONST_STRING(Debug), mt );
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

			this->createMaterialGroup( CONST_STRING(ParticleIntensive), mt );
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

			this->createMaterialGroup( CONST_STRING(ParticleBlend), mt );
		}


		m_megatextures = new Megatextures(2048.f, 2048.f, PF_A8R8G8B8);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::finalize()
	{
		for( TMapTextures::iterator 
			it = m_textures.begin(), 
			it_end = m_textures.end();
		it != it_end;
		++it )
		{
			this->destroyTexture_( it->second );
		}

		m_textures.clear();

		for( TMapMaterialGroup::iterator
			it = m_mapMaterialGroup.begin(),
			it_end = m_mapMaterialGroup.end();
		it != it_end;
		++it )
		{
			delete it->second;
		}

		m_mapMaterialGroup.clear();

		if( m_interface != NULL )
		{
			m_interface->releaseVertexBuffer( m_vbHandle2D );
			m_interface->releaseIndexBuffer( m_vbHandle2D );

			for( TVectorVertexBuffer::iterator
				it = m_vertexBuffer.begin(),
				it_end = m_vertexBuffer.end();
			it != it_end;
			++it )
			{
				VBHandle handle = *it;
				m_interface->releaseVertexBuffer( handle );
			}

			m_vertexBuffer.clear();

			for( TVectorIndexBuffer::iterator
				it = m_indexBuffer.begin(),
				it_end = m_indexBuffer.end();
			it != it_end;
			++it )
			{
				IBHandle handle = *it;
				m_interface->releaseIndexBuffer( handle );
			}

			m_indexBuffer.clear();

			releaseInterfaceSystem( m_interface );
			m_interface = NULL;
		}

		delete m_megatextures;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _viewport, int _bits, bool _fullscreen,
		WindowHandle _winHandle, int _FSAAType, int _FSAAQuality )
	{
		m_windowResolution = _resolution;
		m_contentResolution = _contentResolution;
		m_viewport = _viewport;

		m_fullscreen = _fullscreen;
		
		size_t width = m_windowResolution.getWidth();
		size_t height = m_windowResolution.getHeight();

		m_windowCreated = m_interface->createRenderWindow( width, height, _bits, m_fullscreen, _winHandle,
			m_vsync, _FSAAType, _FSAAQuality );

		if( m_windowCreated == false )
		{
			return false;
		}

		m_debugInfo.textureMemory = 0;
		m_debugInfo.textureCount = 0;
		m_debugInfo.frameCount = 0;

		//////////////////////////////////////////////////////////////////////////
		// Выноси такое в отдельные функции, читать невозможно
		//////////////////////////////////////////////////////////////////////////
		
		size_t null_width = 2;
		size_t null_height = 2;

		RenderTextureInterface * texture  = this->createTexture( null_width, null_height, PF_R8G8B8 );
		
		m_nullTexture = dynamic_cast<RenderTexture*>(texture);
		
		if( m_nullTexture != NULL )
		{
			Rect rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = null_width;
			rect.bottom = null_height;

			int pitch = 0;
			unsigned char* textureData = m_nullTexture->lock( &pitch, rect, false );

			for( size_t it = 0; it != null_height; ++it )
			{
				unsigned char null_color = 0xFF;
				std::fill( textureData, textureData + pitch * it + null_width, null_color );
			}
			
			m_nullTexture->unlock();
		}

		if( this->recreate2DBuffers_( m_maxIndexCount ) == false )
		{
			return false;
		}

		this->restoreRenderSystemStates_();
		this->prepare2D_();

		m_supportA8 = m_interface->supportTextureFormat( PF_A8 );

		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _viewport, bool _fullscreen )
	{
		m_windowResolution = _resolution;
		m_contentResolution = _contentResolution;
		m_viewport = _viewport;
		
		m_fullscreen = _fullscreen;
	
		if( m_windowCreated == false )
		{
			return;
		}

		m_interface->changeWindowMode( _resolution, _fullscreen );

		restoreRenderSystemStates_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::screenshot( RenderTextureInterface* _texture, const mt::vec4f & _rect )
	{
		RenderImageInterface* image = _texture->getImage();

		m_interface->screenshot( image, _rect.buff() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createMaterialGroup( const ConstString & _name, const RenderMaterial & _material )
	{
		TMapMaterialGroup::iterator it_found = m_mapMaterialGroup.find( _name );

		if( it_found != m_mapMaterialGroup.end() )
		{
			MENGE_LOG_ERROR("Error: MaterialGroup '%s' is already created!"
				, _name.c_str()
				);

			return false;
		}

		RenderMaterialGroup * materialGroup = new RenderMaterialGroup;

		materialGroup->filter_group[0] = _material;
		materialGroup->filter_group[0].textureStage[0].addressU = TAM_CLAMP;
		materialGroup->filter_group[0].textureStage[0].addressV = TAM_CLAMP;
		materialGroup->filter_group[0].textureStage[0].filter = TF_ANISOTROPIC;

		materialGroup->filter_group[1] = _material;
		materialGroup->filter_group[1].textureStage[0].addressU = TAM_WRAP;
		materialGroup->filter_group[1].textureStage[0].addressV = TAM_CLAMP;
		materialGroup->filter_group[1].textureStage[0].filter = TF_ANISOTROPIC;

		materialGroup->filter_group[2] = _material;
		materialGroup->filter_group[2].textureStage[0].addressU = TAM_CLAMP;
		materialGroup->filter_group[2].textureStage[0].addressV = TAM_WRAP;
		materialGroup->filter_group[2].textureStage[0].filter = TF_ANISOTROPIC;

		materialGroup->filter_group[3] = _material;
		materialGroup->filter_group[3].textureStage[0].addressU = TAM_WRAP;
		materialGroup->filter_group[3].textureStage[0].addressV = TAM_WRAP;
		materialGroup->filter_group[3].textureStage[0].filter = TF_ANISOTROPIC;

		materialGroup->group[0] = _material;
		materialGroup->group[0].textureStage[0].addressU = TAM_CLAMP;
		materialGroup->group[0].textureStage[0].addressV = TAM_CLAMP;
		materialGroup->group[0].textureStage[0].filter = TF_LINEAR;
		materialGroup->group[0].filterMaterial = &materialGroup->filter_group[0];

		materialGroup->group[1] = _material;
		materialGroup->group[1].textureStage[0].addressU = TAM_WRAP;
		materialGroup->group[1].textureStage[0].addressV = TAM_CLAMP;
		materialGroup->group[1].textureStage[0].filter = TF_LINEAR;
		materialGroup->group[1].filterMaterial = &materialGroup->filter_group[1];

		materialGroup->group[2] = _material;
		materialGroup->group[2].textureStage[0].addressU = TAM_CLAMP;
		materialGroup->group[2].textureStage[0].addressV = TAM_WRAP;
		materialGroup->group[2].textureStage[0].filter = TF_LINEAR;
		materialGroup->group[2].filterMaterial = &materialGroup->filter_group[2];

		materialGroup->group[3] = _material;
		materialGroup->group[3].textureStage[0].addressU = TAM_WRAP;
		materialGroup->group[3].textureStage[0].addressV = TAM_WRAP;
		materialGroup->group[3].textureStage[0].filter = TF_LINEAR;
		materialGroup->group[3].filterMaterial = &materialGroup->filter_group[3];
		
		
		m_mapMaterialGroup.insert( std::make_pair(_name, materialGroup) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderMaterialGroup * RenderEngine::getMaterialGroup( const ConstString & _name ) const
	{
		TMapMaterialGroup::const_iterator it_found = m_mapMaterialGroup.find( _name );

		if( it_found == m_mapMaterialGroup.end() )
		{
			MENGE_LOG_ERROR("Error: not exsist RenderMaterial '%s'"
				, _name.c_str()
				);

			return 0;
		}

		return it_found->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::removeMaterialGroup( const ConstString & _name )
	{
		TMapMaterialGroup::iterator it_found = m_mapMaterialGroup.find( _name );

		if( it_found == m_mapMaterialGroup.end() )
		{
			MENGE_LOG_ERROR("Error: not exsist RenderMaterial '%s'"
				, _name.c_str()
				);

			return;
		}

		delete it_found->second;

		m_mapMaterialGroup.erase( it_found );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface * RenderEngine::createTexture( size_t _width, size_t _height, PixelFormat _format )
	{
		MENGE_LOG_INFO( "Creating texture %dx%d %d"
			, _width
			, _height
			, _format 
			);

		size_t hwWidth;
		size_t hwHeight;
		PixelFormat hwFormat = _format;

		RenderImageInterface * image = m_interface->createImage( _width, _height, hwWidth, hwHeight, hwFormat );
				
		if( image == NULL )
		{
			MENGE_LOG_ERROR( "RenderEngine::createTexture_ couldn't create image %dx%d"
				, _width
				, _height
				);

			return NULL;
		}

		size_t id = ++m_idEnumerator;

		RenderTextureInterface * texture = new RenderTexture( image, _width, _height, _format, hwWidth, hwHeight, hwFormat, id );

		size_t memroy_size = texture->getMemoryUse();

		m_debugInfo.textureMemory += memroy_size;
		++m_debugInfo.textureCount;

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface * RenderEngine::createMegatexture( size_t _width, size_t _height, PixelFormat _format )
	{
		PixelFormat format = m_megatextures->getPixelFormat();

		if( _format != format )
		{
			RenderTextureInterface * texture = this->createTexture( _width, _height, _format );

			return texture;
		}

		RenderTextureInterface * texture = m_megatextures->createTexture( _width, _height );

		size_t memroy_size = texture->getMemoryUse();

		m_debugInfo.textureMemory += memroy_size;
		++m_debugInfo.textureCount;

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface * RenderEngine::createSubTexture( RenderTextureInterface * _texture, const Rect & _rect, RenderTextureInterfaceListener * _listener )
	{
		MENGE_LOG_INFO( "Creating sub texture [%d %d - %d %d]"
			, _rect.left
			, _rect.top
			, _rect.right
			, _rect.bottom
			);

		size_t id = ++m_idEnumerator;

		RenderSubTexture * texture = new RenderSubTexture( _texture, _rect, id, _listener );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::saveImage( RenderTextureInterface* _image, const ConstString & _fileSystemName, const WString & _filename )
	{
		FileOutputStreamInterface * stream = FileEngine::get()
			->openOutputFile( _fileSystemName, _filename );

		if( stream == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : can't create file '%s'"
				, _filename.c_str() 
				);

			return false;
		}

		ImageEncoderInterface * imageEncoder = CodecEngine::get()
			->createEncoderT<ImageEncoderInterface>( ConstString("pngImage"), stream );

		if( imageEncoder == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : can't create encoder for filename '%s'"
				, _filename.c_str() 
				);

			stream->close();

			return false;
		}

		ImageCodecDataInfo dataInfo;
		dataInfo.format = _image->getHWPixelFormat();
		dataInfo.height = _image->getHeight();
		dataInfo.width = _image->getWidth();
		dataInfo.depth = 1;
		dataInfo.num_mipmaps = 0;
		dataInfo.flags = 0;
		dataInfo.size = 0;	// we don't need this
		int pitch = 0;

		Rect rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = dataInfo.width;
		rect.bottom = dataInfo.height;
		
		unsigned char * buffer = _image->lock( &pitch, rect, true );

		/*unsigned char* lockBuffer = new unsigned char[ imgData.height * pitch ];

		size_t mPitch = Menge::PixelUtil::getNumElemBytes( imgData.format ) * imgData.width;
		for( size_t i = 0; i != imgData.height; i++ )
		{
		std::copy( buffer, buffer + mPitch, lockBuffer );
		//memcpy( _dstData, _srcData, width * 4 );
		lockBuffer += mPitch;
		buffer += pitch;
		}
		_image->unlock();*/

		//lockBuffer -= mPitch * imgData.height;

		ImageCodecOptions options;
		options.flags |= DF_CUSTOM_PITCH;
		options.flags |= (pitch << 16);
		imageEncoder->setOptions( &options );

		unsigned int bytesWritten = imageEncoder->encode( buffer, &dataInfo );

		_image->unlock();

		//imageEncoder->release();

		if( bytesWritten == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : Error while encoding image data" );
			return false;
		}

		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	//bool RenderEngine::validTexture( const ConstString& _pakName, const WString& _filename, const ConstString& _codec )
	//{
	//	bool exist = FileEngine::get()
	//		->existFile( _pakName, _filename );

	//	return exist;
	//}
	////////////////////////////////////////////////////////////////////////////
	//RenderTextureInterface * RenderEngine::getTexture( const ConstString & _name ) const
	//{
	//	TMapTextures::const_iterator it_find = m_textures.find( _name );
	//	
	//	if( it_find == m_textures.end() )
	//	{
	//		return NULL;
	//	}

	//	it_find->second->addRef();
	//	return it_find->second;
	//}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::cacheFileTexture( const WString& _filename, RenderTextureInterface* _texture )
	{
		_texture->setFileName( _filename );

		m_textures.insert( std::make_pair( _filename, _texture ) );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface* RenderEngine::loadTexture( const ConstString& _pakName, const WString & _filename, const ConstString& _codec )
	{
		TMapTextures::iterator it_find = m_textures.find( _filename );
	
		if( it_find != m_textures.end() )
		{
			it_find->second->addRef();

			return it_find->second;
		}
		
		FileInputStreamInterface * stream = FileEngine::get()
			->openInputFile( _pakName, _filename );
		
		if( stream == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::loadTexture: Image file '%S' was not found"
				, _filename.c_str() 
				);

			return NULL;
		}

		ImageDecoderInterface * imageDecoder = CodecEngine::get()
			->createDecoderT<ImageDecoderInterface>( _codec, stream );

		if( imageDecoder == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::loadTexture: Image decoder for file '%S' was not found"
				, _filename.c_str() 
				);

			stream->close();

			return NULL;
		}

		const ImageCodecDataInfo* dataInfo = imageDecoder->getCodecDataInfo();

		if( dataInfo->format == PF_UNKNOWN )
		{
			MENGE_LOG_ERROR( "RenderEngine::loadTexture: Image file '%S' Invalid format [PF_UNKNOWN]"
				, _filename.c_str()
				);

			imageDecoder->destroy();

			stream->close();

			return NULL;
		}
		
		RenderTextureInterface * texture = this->createTexture( dataInfo->width, dataInfo->height, dataInfo->format );
		
		if( texture == NULL )
		{
			imageDecoder->destroy();

			stream->close();

			return NULL;
		}

		const Rect & rect = texture->getRect();
		this->loadTextureRectImageData( texture, rect, imageDecoder );

		imageDecoder->destroy();

		stream->close();
		
		this->cacheFileTexture( _filename, texture );
		
		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface* RenderEngine::loadMegatexture( const ConstString& _pakName, const WString& _filename, const ConstString& _codec )
	{
		TMapTextures::iterator it_find = m_textures.find( _filename );

		if( it_find != m_textures.end() )
		{
			it_find->second->addRef();

			return it_find->second;
		}

		FileInputStreamInterface * stream = FileEngine::get()
			->openInputFile( _pakName, _filename );

		if( stream == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::loadTexture: Image file '%S' was not found"
				, _filename.c_str() 
				);

			return NULL;
		}

		ImageDecoderInterface * imageDecoder = CodecEngine::get()
			->createDecoderT<ImageDecoderInterface>( _codec, stream );

		if( imageDecoder == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::loadTexture: Image decoder for file '%S' was not found"
				, _filename.c_str() 
				);

			stream->close();

			return NULL;
		}

		const ImageCodecDataInfo* dataInfo = imageDecoder->getCodecDataInfo();

		if( dataInfo->format == PF_UNKNOWN )
		{
			MENGE_LOG_ERROR( "RenderEngine::loadTexture: Image file '%S' Invalid format [PF_UNKNOWN]"
				, _filename.c_str()
				);

			imageDecoder->destroy();

			stream->close();

			return NULL;
		}

		RenderTextureInterface * texture = this->createMegatexture( dataInfo->width, dataInfo->height, dataInfo->format );

		if( texture == NULL )
		{
			imageDecoder->destroy();

			stream->close();

			return NULL;
		}

		const Rect & rect = texture->getRect();
		this->loadTextureRectImageData( texture, rect, imageDecoder );

		imageDecoder->destroy();

		stream->close();

		this->cacheFileTexture( _filename, texture );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface* RenderEngine::loadTextureCombineRGBAndAlpha( const ConstString& _pakName, const WString & _fileNameRGB, const WString & _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha )
	{		
		//WString textureName = _fileNameAlpha;

		TMapTextures::iterator it_find = m_textures.find( _fileNameRGB );

		if( it_find != m_textures.end() )
		{
			it_find->second->addRef();
			return it_find->second;
		}

		////////////////////////////////////// init RGB Decoder
		///Load RGB data
		FileInputStreamInterface * streamRGB = FileEngine::get()
			->openInputFile( _pakName, _fileNameRGB );

		if( streamRGB == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::loadTextureCombineRGBAndAlpha: Image file with RGB data '%S' was not found"
				, _fileNameRGB.c_str() 
				);

			return NULL;
		}

		///Get RGB Decoder
		ImageDecoderInterface * imageDecoderRGB = CodecEngine::get()
			->createDecoderT<ImageDecoderInterface>( _codecRGB, streamRGB );

		if( imageDecoderRGB == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image decoder for file '%s' was not found"
				, _fileNameRGB.c_str() 
				);

			streamRGB->close();

			return NULL;
		}
		
		/// RgbInfo 
		const ImageCodecDataInfo* dataInfoRGB = imageDecoderRGB->getCodecDataInfo();

		if( dataInfoRGB->format == PF_UNKNOWN )
		{
			MENGE_LOG_ERROR( "RenderEngine::Error: Invalid image format '%s'"
				, _fileNameRGB.c_str() 
				);

			imageDecoderRGB->destroy();

			streamRGB->close();

			return NULL;
		}
		
		////////////////////////////////////// init Alpha Decoder
		///Load Alpha data
		FileInputStreamInterface * streamAlpha = FileEngine::get()
			->openInputFile( _pakName, _fileNameAlpha );

		if( streamAlpha == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image file with alpha channel data '%s' was not found"
				, _fileNameAlpha.c_str() 
				);

			return NULL;
		}

		///Get Alpha Decoder
		ImageDecoderInterface * imageDecoderAlpha = CodecEngine::get()
			->createDecoderT<ImageDecoderInterface>( _codecAlpha, streamAlpha );

		if( imageDecoderAlpha == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image decoder for file '%s' was not found"
				, _fileNameAlpha.c_str() 
				);

			streamAlpha->close();

			return NULL;
		}
		
		///Alpha Info
		const ImageCodecDataInfo* dataInfoAlpha = imageDecoderAlpha->getCodecDataInfo();

		if( dataInfoAlpha->format == PF_UNKNOWN )
		{
			MENGE_LOG_ERROR( "RenderEngine::Error: Invalid image format '%s'"
				, _fileNameAlpha.c_str() 
				);

			imageDecoderAlpha->destroy();

			streamAlpha->close();

			return NULL;
		}
			
		LogServiceInterface * logService = LogEngine::get();
		
		ImageDecoderCombinerRGBAndAlpha *  imageCombiner = new  ImageDecoderCombinerRGBAndAlpha( imageDecoderRGB, imageDecoderAlpha, logService );
		/*
		///Get Combiner Decoder
		ImageDecoderInterface * imageCombiner = CodecEngine::get()
			->createDecoderCombinerT<ImageDecoderInterface>( "combinedImage", imageDecoderRGB, imageDecoderAlpha );
		*/
		const ImageCodecDataInfo* dataInfo = imageCombiner->getCodecDataInfo();

		RenderTextureInterface * texture = this->createTexture( dataInfo->width, dataInfo->height, dataInfo->format );

		if( texture == NULL )
		{			
			imageDecoderRGB->destroy();
			imageDecoderAlpha->destroy();
			imageCombiner->destroy();
			
			streamAlpha->close();
			streamRGB->close();

			return NULL;
		}

		const Rect & rect = texture->getRect();
		this->loadTextureRectImageData( texture, rect, imageCombiner );

		//texture->loadImageData( imageCombiner );
		
		imageDecoderRGB->destroy();
		imageDecoderAlpha->destroy();
		imageCombiner->destroy();
		
		streamAlpha->close();
		streamRGB->close();

		this->cacheFileTexture( _fileNameRGB, texture );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseTexture( RenderTextureInterface* _texture )
	{
		if( _texture == NULL )
		{
			return;
		}

		if( _texture->decRef() > 0 )
		{
			return;
		}

		const WString & filename = _texture->getFileName();
		
		m_textures.erase( filename );

		this->destroyTexture_( _texture );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::destroyTexture_( RenderTextureInterface * _texture )
	{
		size_t memroy_size = _texture->getMemoryUse();

		m_debugInfo.textureMemory -= memroy_size;
		--m_debugInfo.textureCount;
		
		_texture->destroyImage();
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::loadTextureRectImageData( RenderTextureInterface * _texture, const Rect & _rect, ImageDecoderInterface* _imageDecoder )
	{
		int pitch = 0;
		unsigned char * textureBuffer = _texture->lock( &pitch, _rect, false );

		if( textureBuffer == NULL )
		{
			MENGE_LOG_ERROR("RenderEngine::loadTextureImageData Invalid lock");

			return false;
		}

		ImageCodecOptions options;

		const ImageCodecDataInfo* dataInfo = _imageDecoder->getCodecDataInfo();

		PixelFormat hwPixelFormat = _texture->getHWPixelFormat();

		if( dataInfo->format == PF_R8G8B8
			&& hwPixelFormat == PF_X8R8G8B8 )
		{
			options.flags |= DF_COUNT_ALPHA;
		}

		size_t width = _texture->getWidth();

		if( pitch != width )
		{
			options.pitch = pitch;

			options.flags |= DF_CUSTOM_PITCH;
		}

		_imageDecoder->setOptions( &options );
		
		bool result = this->loadBufferImageData( textureBuffer, pitch, hwPixelFormat, _imageDecoder );

		_texture->unlock();

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::loadBufferImageData( unsigned char* _textureBuffer, size_t _texturePitch, PixelFormat _hwPixelFormat, ImageDecoderInterface * _imageDecoder )
	{
		const ImageCodecDataInfo * data = _imageDecoder->getCodecDataInfo();

		unsigned int bufferSize = _texturePitch * data->height;
		unsigned int b = _imageDecoder->decode( _textureBuffer, bufferSize );

		this->sweezleAlpha_( _textureBuffer, _texturePitch, _hwPixelFormat, data );
		//this->imageQuality_( _textureBuffer, _texturePitch );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::sweezleAlpha_( unsigned char* _textureBuffer, size_t _texturePitch, PixelFormat _hwPixelFormat, const ImageCodecDataInfo * _dataInfo )
	{
		// need to sweezle alpha
		if( _dataInfo->format == PF_A8
			&& _hwPixelFormat == PF_A8R8G8B8 )
		{
			for( size_t h = _dataInfo->height - 1; h != -1; --h )
			{
				size_t hp = h * _texturePitch;

				for( size_t w = _dataInfo->width - 1; w != -1; --w )
				{
					_textureBuffer[hp+w*4+3] = _textureBuffer[hp+w];
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::imageQuality_( unsigned char * _textureBuffer, size_t _texturePitch )
	{
		// copy pixels on the edge for better image quality
		//if( m_hwWidth > m_width )
		//{
		//	unsigned char* image_data = _textureBuffer;
		//	unsigned int pixel_size = _texturePitch / m_hwWidth;

		//	for( size_t i = 0; i != m_height; ++i )
		//	{
		//		std::copy( image_data + (m_width - 1) * pixel_size, 
		//			image_data + m_width * pixel_size,
		//			image_data + m_width * pixel_size );

		//		image_data += _texturePitch;
		//	}
		//}

		//if( m_hwHeight > m_height )
		//{
		//	unsigned char* image_data = _textureBuffer;
		//	unsigned int pixel_size = _texturePitch / m_hwWidth;
		//	std::copy( image_data + (m_height - 1) * _texturePitch,
		//		image_data + m_height * _texturePitch,
		//		image_data + m_height * _texturePitch );
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	Resolution RenderEngine::getBestDisplayResolution( const Resolution & _resolution, float _aspect )
	{
		TVectorResolutions resolutions;
		m_interface->getResolutions( resolutions );

		float rx = float(_resolution.getWidth());
		float ry = float(_resolution.getHeight());

		float needWidth = ry * _aspect;

		typedef std::map<int, TVectorResolutions> TMapResolutions;
		TMapResolutions resMap;

		size_t count = resolutions.size() / 2;

		for( size_t i = 0; i < count; i++ )
		{
			if( fabsf( ( static_cast<float>( resolutions[2*i] ) / resolutions[2*i+1] ) - _aspect ) < 0.01f )
			{
				resMap[ resolutions[2*i + 1] ].push_back( resolutions[2*i] );
			}
		}

		bool done = false;

		Resolution bestResolution;

		for( TMapResolutions::iterator 
			it = resMap.begin(),
			it_end = resMap.end();
		it != it_end; 
		++it )
		{
			size_t height = _resolution.getHeight();
			if( it->first < static_cast<int>(height) ) 
			{
				continue;
			}

			for( TVectorResolutions::size_type 
				i = 0,
				i_end = it->second.size();
			i != i_end; 
			++i)
			{
				if( it->second[i] >= static_cast<int>(needWidth) )
				{
					bestResolution.setWidth( it->second[i] );
					bestResolution.setHeight( it->first );
					done = true;
					break;
				}
			}
			if( done ) break;
		}

		return bestResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onDeviceRestored()
	{
		//const Resolution & resolution = Holder<Game>::get()
		//	->getResolution();

		//Holder<Application>::get()
		//	->notifyWindowModeChanged( resolution[0], resolution[1], m_fullscreen );
		this->refillIndexBuffer2D_();
		this->restoreRenderSystemStates_();
	}
	//////////////////////////////////////////////////////////////////////////
	/*void RenderEngine::onWindowActive( bool _active )
	{
	if( m_windowCreated )
	{
	m_interface->onWindowActive( _active );
	}
	}*/
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onWindowClose()
	{
		if( m_windowCreated )
		{
			m_interface->onWindowClose();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::beginScene()
	{	
		for( TVectorRenderPass::iterator 
			it = m_passes.begin(), 
			it_end = m_passes.end();
		it != it_end;
		++it )
		{
			this->releaseRenderPass_( *it );
		}
		
		m_passes.clear();

		m_currentPass = NULL;
		m_currentRenderCamera = NULL;

		m_currentRenderTarget = Consts::get()->c_Window;
		m_renderTargetResolution = m_windowResolution;
		m_dipCount = 0;

		if( m_interface->beginScene() == false )
		{
			return false;
		}
		//m_interface->clearFrameBuffer( FBT_COLOR );
		//m_interface->setRenderViewport( m_currentRenderViewport );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::endScene()
	{
		this->flushRender_();

		m_interface->endScene();

		m_debugInfo.dips = m_dipCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::swapBuffers()
	{
		m_interface->swapBuffers();

		m_debugInfo.frameCount += 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderTarget( const ConstString& _target, bool _clear )
	{
		m_currentRenderTarget = _target;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& RenderEngine::getRenderTarget() const
	{
		return m_currentRenderTarget;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::isWindowCreated() const
	{
		return m_windowCreated;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderPass_( const RenderObject* _renderObject )
	{
		if( _renderObject->dipIndiciesNum == 0 
			|| _renderObject->dipVerticesNum == 0 )
		{
			return;
		}

		if( m_currentTextureStages > _renderObject->textureStages )
		{
			for( size_t stageId = _renderObject->textureStages; stageId != m_currentTextureStages; ++stageId )
			{
				this->disableTextureStage_( stageId );
			}
		}

		const RenderMaterial* material = _renderObject->material;

		m_currentTextureStages = _renderObject->textureStages;

		for( size_t stageId = 0; stageId != m_currentTextureStages; ++stageId )
		{
			const RenderTextureInterface * texture = _renderObject->textures[stageId];

			if( texture == NULL )
			{
				texture = m_nullTexture;
			}
		
			if( texture->getId() != m_currentTexturesID[stageId] || m_currentTexturesID[stageId] != 0 )
			{
				m_currentTexturesID[stageId] = texture->getId();
				RenderImageInterface * image = texture->getImage();

				m_interface->setTexture( stageId, image );
			}

			if( m_currentMaterial != material )
			{
				RenderTextureStage & current_stage = m_currentTextureStage[stageId];
				const RenderTextureStage & stage = material->textureStage[stageId];

				if( current_stage.filter != stage.filter )
				{
					current_stage.filter = stage.filter;

					m_interface->setTextureStageFilter( stageId, TFT_MAGNIFICATION, current_stage.filter );
					m_interface->setTextureStageFilter( stageId, TFT_MINIFICATION, current_stage.filter );
				}

				if( current_stage.addressU != stage.addressU
					|| current_stage.addressV != stage.addressV )
				{
					current_stage.addressU = stage.addressU;
					current_stage.addressV = stage.addressV;

					m_interface->setTextureAddressing( stageId
						, current_stage.addressU
						, current_stage.addressV );
				}

				if( current_stage.colorOp != stage.colorOp
					|| current_stage.colorArg1 != stage.colorArg1
					|| current_stage.colorArg2 != stage.colorArg2 )
				{
					current_stage.colorOp = stage.colorOp;
					current_stage.colorArg1 = stage.colorArg1;
					current_stage.colorArg2 = stage.colorArg2;

					m_interface->setTextureStageColorOp( stageId
						, current_stage.colorOp
						, current_stage.colorArg1
						, current_stage.colorArg2 );
				}

				if( current_stage.alphaOp != stage.alphaOp
					|| current_stage.alphaArg1 != stage.alphaArg1
					|| current_stage.alphaArg2 != stage.alphaArg2 )
				{
					current_stage.alphaOp = stage.alphaOp;
					current_stage.alphaArg1 = stage.alphaArg1;
					current_stage.alphaArg2 = stage.alphaArg2;

					m_interface->setTextureStageAlphaOp( stageId
						, current_stage.alphaOp
						, current_stage.alphaArg1
						, current_stage.alphaArg2 );
				}
			}
		}

		if( m_currentMaterial != material )
		{
			if( m_depthBufferWriteEnable != material->depthBufferWriteEnable )
			{
				m_depthBufferWriteEnable = material->depthBufferWriteEnable;
				m_interface->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
			}

			if( m_alphaTestEnable != material->alphaTestEnable )
			{
				m_alphaTestEnable = material->alphaTestEnable;
				m_interface->setAlphaTestEnable( m_alphaTestEnable );
			}

			if( m_alphaBlendEnable != material->alphaBlendEnable )
			{
				m_alphaBlendEnable = material->alphaBlendEnable;
				m_interface->setAlphaBlendEnable( m_alphaBlendEnable );
			}

			if( m_currentBlendSrc != material->blendSrc )
			{
				m_currentBlendSrc = material->blendSrc;
				m_interface->setSrcBlendFactor( m_currentBlendSrc );
			}

			if( m_currentBlendDst != material->blendDst )
			{
				m_currentBlendDst = material->blendDst;
				m_interface->setDstBlendFactor( m_currentBlendDst );
			}
		}

		if( m_currentIBHandle != _renderObject->ibHandle || 
			m_currentBaseVertexIndex != _renderObject->baseVertexIndex )
		{
			m_currentIBHandle = _renderObject->ibHandle;
			m_currentBaseVertexIndex = _renderObject->baseVertexIndex;

			m_interface->setIndexBuffer( m_currentIBHandle, m_currentBaseVertexIndex );
		}

		m_interface->drawIndexedPrimitive( 
			_renderObject->primitiveType, 
			_renderObject->baseVertexIndex, 
			_renderObject->minIndex,
			_renderObject->dipVerticesNum, 
			_renderObject->startIndex, 
			_renderObject->dipIndiciesNum 
			);

		m_currentMaterial = material;

		++m_dipCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::disableTextureStage_( size_t _stage )
	{
		RenderTextureStage & stage = m_currentTextureStage[_stage];

		m_currentTexturesID[_stage] = 0;

		stage.colorOp = TOP_DISABLE;
		stage.colorArg1 = TARG_TEXTURE;
		stage.colorArg2 = TARG_DIFFUSE;

		m_interface->setTexture( _stage, NULL );

		m_interface->setTextureStageColorOp( _stage
			, stage.colorOp
			, stage.colorArg1
			, stage.colorArg2
			);

		m_interface->setTextureMatrix( _stage, NULL );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::orthoOffCenterLHMatrix_( mt::mat4f& _out, float l, float r, float b, float t, float zn, float zf )
	{
		mt::ident_m4( _out );

		float inv_lr = 1.0f / ( l - r );
		float inv_bt = 1.0f / ( b - t );
		float inv_znzf = 1.0f / ( zn - zf );
		_out[0][0] = -2.0f * inv_lr;
		_out[1][1] = -2.0f * inv_bt;
		_out[2][2] = -1.0f * inv_znzf;
		_out[3][0] = ( l + r ) * inv_lr;
		_out[3][1] = ( t + b ) * inv_bt;
		_out[3][2] = zn * inv_znzf;
		_out[3][3] = 1.0f;

	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::restoreRenderSystemStates_()
	{
		//mt::mat4f ident;
		m_vbPos = 0;
		
		mt::mat4f viewTransform;
		mt::mat4f projTransform;
		mt::mat4f worldTransform;

		mt::ident_m4( viewTransform );
		mt::ident_m4( projTransform );
		mt::ident_m4( worldTransform );

		m_interface->setVertexBuffer( m_currentVBHandle );
		m_interface->setIndexBuffer( m_currentIBHandle, m_currentBaseVertexIndex );
		m_interface->setVertexDeclaration( sizeof(Vertex2D), Vertex2D_declaration );
		m_interface->setProjectionMatrix( projTransform );
		m_interface->setModelViewMatrix( viewTransform );
		m_interface->setWorldMatrix( worldTransform );
		m_interface->setCullMode( CM_CULL_NONE );
		m_interface->setFillMode( FM_SOLID );
		m_interface->setDepthBufferTestEnable( false );
		m_interface->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
		m_interface->setDepthBufferCmpFunc( CMPF_LESS_EQUAL );
		m_interface->setAlphaTestEnable( m_alphaTestEnable );
		m_interface->setAlphaBlendEnable( m_alphaBlendEnable );
		m_interface->setAlphaCmpFunc( CMPF_GREATER_EQUAL, 0x01 );
		m_interface->setLightingEnable( false );
		
		for( int i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			RenderTextureStage & stage = m_currentTextureStage[i];

			//stage = TextureStage();

			m_interface->setTextureAddressing( i, stage.addressU, stage.addressV );

			m_interface->setTextureStageColorOp( i
				, stage.colorOp
				, stage.colorArg1
				, stage.colorArg2 
				);

			m_interface->setTextureStageAlphaOp( i
				, stage.alphaOp
				, stage.alphaArg1
				, stage.alphaArg2 );

			//ETextureFilter tFilter = m_textureFiltering ? TF_POINT : TF_NONE;

			m_interface->setTextureStageFilter( i, TFT_MIPMAP, TF_NONE );
			m_interface->setTextureStageFilter( i, TFT_MAGNIFICATION, stage.filter );
			m_interface->setTextureStageFilter( i, TFT_MINIFICATION, stage.filter );
			
			// skip texture matrix
			//m_uvMask[i] = NULL;
			//stage.matrix = NULL;
			m_currentMatrixUV[i] = 0;
			m_currentMaskUV[i] = 0;

			m_interface->setTextureMatrix( i, NULL );
		}

		//m_currentBlendSrc = BF_ONE;
		m_interface->setSrcBlendFactor( m_currentBlendSrc );

		//m_currentBlendDst = BF_ZERO;
		m_interface->setDstBlendFactor( m_currentBlendDst );

		float viewport_width = m_viewport.getWidth();
		float viewport_height = m_viewport.getHeight();

		float width_scale = viewport_width / float(m_contentResolution.getWidth());
		float height_scale = viewport_height / float(m_contentResolution.getHeight());

		m_renderScale.x = width_scale;
		m_renderScale.y = height_scale;

		m_renderOffset = m_viewport.begin;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeProjectionOrthogonal( mt::mat4f& _projectionMatrix, const Viewport & _viewport )
	{
		m_interface->makeProjectionOrthogonal( _projectionMatrix, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, 0.0f, 1.0f );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf )
	{
		m_interface->makeProjectionPerspective( _projectionMatrix, _fov, _aspect, zn, zf );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeViewMatrixFromViewport( mt::mat4f& _viewMatrix, const Viewport & _viewport )
	{
		mt::mat4f wm;
		mt::ident_m4( wm );

		//mt::vec2f size;
		//_viewport.getSize( size );

		//float scale_width = (size.x / float(m_contentResolution.getWidth()));
		//float scale_height = (size.y / float(m_contentResolution.getHeight()));

		//wm.v0.x = 1.f / scale_width;
		//wm.v1.y = 1.f / scale_height;
		//wm.v3.x = _viewport.begin.x;
		//wm.v3.y = _viewport.begin.y;

		mt::inv_m4( _viewMatrix, wm );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderPass * RenderEngine::createRenderPass_()
	{
		RenderPass* pass = m_poolRenderPass.get();
		
		pass->renderObjects.clear();

		m_passes.push_back( pass );

		return pass;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderEngine::DebugInfo& RenderEngine::getDebugInfo() const
	{
		return m_debugInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::hasTexture( const WString & _filename )
	{
		TMapTextures::iterator it_find = m_textures.find( _filename );

		if( it_find == m_textures.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::resetFrameCount()
	{
		m_debugInfo.frameCount = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::render_()
	{
		if( m_passes.empty() == true )	// nothing to render
		{
			return;
		}

		for( TVectorRenderPass::iterator 
			rit = m_passes.begin(), 
			rit_end = m_passes.end();
		rit != rit_end;
		++rit )
		{
			RenderPass * renderPass = *rit;

			const RenderCameraInterface * camera = renderPass->camera;

			const ConstString& renderTarget = camera->getRenderTarget();
			
			if( renderTarget != m_currentRenderTarget && renderTarget.empty() == false )
			{
				m_currentRenderTarget = renderTarget;
				m_interface->setRenderTarget( NULL, true );

				m_renderTargetResolution = m_windowResolution;
			}

			Viewport renderViewport;

			const Viewport & viewport = camera->getViewport();

			renderViewport.begin = m_renderOffset + viewport.begin * m_renderScale;
			renderViewport.end = m_renderOffset + viewport.end * m_renderScale;
			
			m_interface->setViewport( renderViewport );

			const mt::mat4f & viewMatrix = camera->getViewMatrix();
			m_interface->setModelViewMatrix( viewMatrix );

			const mt::mat4f & projectionMatrix = camera->getProjectionMatrix();
			m_interface->setProjectionMatrix( projectionMatrix );

			const TVectorRenderObject & renderObjects = renderPass->renderObjects;

			for( TVectorRenderObject::const_iterator 
				it = renderObjects.begin(), 
				it_end = renderObjects.end();
			it != it_end; 
			++it )
			{
				const RenderObject* renderObject = &(*it);

				this->renderPass_( renderObject );
			}			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderObject2D( const RenderCameraInterface * _camera, const RenderMaterial* _material, const RenderTextureInterface* const * _textures, mt::mat4f * const * _matrixUV, size_t _texturesNum,
										const Vertex2D* _vertices, size_t _verticesNum,
										ELogicPrimitiveType _type, size_t _indicesNum, IBHandle _ibHandle )
	{
		if( _camera == NULL )
		{
			MENGE_LOG_ERROR("RenderEngine::renderObject2D camera == NULL"
				);

			return;
		}

		if( _material == NULL )
		{
			MENGE_LOG_ERROR("RenderEngine::renderObject2D _material == NULL"
				);

			return;
		}

		if( m_currentRenderCamera != _camera )
		{
			m_currentRenderCamera = _camera;

			m_currentPass = this->createRenderPass_();

			m_currentPass->camera = m_currentRenderCamera;			
		}

		RenderObject renderObject;
		RenderObject* ro = &renderObject;

		ro->material = _material;

		ro->textureStages = _texturesNum;

		ro->logicPrimitiveType = _type;

		ro->dipIndiciesNum = m_primitiveIndexStride[_type] * _verticesNum / m_primitiveVertexStride[_type];
		ro->ibHandle = m_ibHandle2D;
	
		ro->vertexData = _vertices;
		ro->verticesNum = _verticesNum;

		ro->baseVertexIndex = 0;

		for( size_t i = 0; i != _texturesNum; ++i )
		{
			if( _textures == NULL )
			{
				ro->textures[i] = 0;
			}
			else
			{
				ro->textures[i] = _textures[i];
			}
			
			if( _matrixUV == NULL )
			{
				ro->matrixUV[i] = NULL;
			}
			else
			{
				ro->matrixUV[i] = _matrixUV[i];
			}			 
		}

		switch( _type )
		{
		case LPT_QUAD:
		//case LPT_TRIANGLE:
			ro->primitiveType = PT_TRIANGLELIST;
			break;
		case LPT_LINE:
		//case LPT_RECTANGLE:
			ro->primitiveType = PT_LINESTRIP;
			break;
		default:
			ro->primitiveType = PT_TRIANGLELIST;
		}

		m_currentPass->renderObjects.push_back( *ro );
		//if( m_currentPass->renderObjects.empty() == true )
		//{
		//	if( this->batchRenderObject_( ro, NULL, m_vertexBufferPos ) == false )
		//	{
		//		m_currentPass->renderObjects.push_back( *ro );
		//		//*m_batchedRenderObject = *ro;
		//	}
		//}
		//else
		//{
		//	RenderObject * batchedRenderObject = &m_currentPass->renderObjects.back();

		//	if( this->batchRenderObject_( ro, batchedRenderObject, m_vertexBufferPos ) == false )
		//	{
		//		m_currentPass->renderObjects.push_back( *ro );
		//		//*m_batchedRenderObject = *ro;
		//	}
		//}

		//m_currentPass->renderObjects.push_back( *ro );
	}
	//////////////////////////////////////////////////////////////////////////
	VBHandle RenderEngine::createVertexBuffer( const Vertex2D * _buffer, size_t _count )
	{
		VBHandle vbHandle = m_interface->createVertexBuffer( _count, sizeof(Vertex2D) );

		if( vbHandle == 0 )
		{
			return 0;
		}

		m_vertexBuffer.push_back(vbHandle);

		this->updateVertexBuffer( vbHandle, _buffer, _count );
		
		return vbHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	IBHandle RenderEngine::createIndicesBuffer( const unsigned short * _buffer, size_t _count )
	{
		IBHandle ibHandle = m_interface->createIndexBuffer(_count);

		if( ibHandle == 0 )
		{
			return 0;
		}

		m_indexBuffer.push_back( ibHandle );

		this->updateIndicesBuffer( ibHandle, _buffer, _count );

		return ibHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseVertexBuffer( VBHandle _handle )
	{
		TVectorVertexBuffer::iterator it_found = std::find( m_vertexBuffer.begin(), m_vertexBuffer.end(), _handle );

		if( it_found == m_vertexBuffer.end() )
		{
			MENGE_LOG_ERROR( "RenderEngine: failed to release vertex buffer" );
			return;
		}

		m_vertexBuffer.erase( it_found );

		m_interface->releaseVertexBuffer( _handle );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseIndicesBuffer( IBHandle _handle )
	{
		TVectorIndexBuffer::iterator it_found = std::find( m_indexBuffer.begin(), m_indexBuffer.end(), _handle );

		if( it_found == m_indexBuffer.end() )
		{
			MENGE_LOG_ERROR( "RenderEngine: failed to release index buffer" );
			return;
		}

		m_indexBuffer.erase( it_found );

		m_interface->releaseIndexBuffer( _handle );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::updateVertexBuffer( VBHandle _handle, const Vertex2D * _buffer, size_t _count  )
	{
		void * vbuffer = m_interface->lockVertexBuffer( 
			_handle,
			0,
			_count * sizeof(Vertex2D), 
			LOCK_DISCARD );

		if( vbuffer == NULL )
		{
			MENGE_LOG_ERROR("RenderEngine: failed to lock vertex buffer");
			return;
		}

		std::copy( _buffer + 0, _buffer + _count, static_cast<Vertex2D*>(vbuffer) );

		if( m_interface->unlockVertexBuffer( _handle ) == false )
		{
			MENGE_LOG_ERROR( "RenderEngine: failed to unlock vertex buffer" );
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::updateIndicesBuffer( IBHandle _handle, const unsigned short * _buffer, size_t _count )
	{
		uint16* ibuffer = m_interface->lockIndexBuffer( _handle );

		if( ibuffer == NULL )
		{
			MENGE_LOG_ERROR("RenderEngine: failed to lock vertex buffer");
			return;
		}

		std::copy( _buffer + 0, _buffer + _count, ibuffer );

		if( m_interface->unlockIndexBuffer( _handle ) == false )
		{
			MENGE_LOG_ERROR( "RenderEngine: failed to unlock vertex buffer" );
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderEngine::makeBatch_( size_t _offset )
	{
		size_t vbPos = _offset;

		for( TVectorRenderPass::iterator 
			it = m_passes.begin(), 
			it_end = m_passes.end();
		it != it_end;
		++it )
		{
			RenderPass * pass = (*it);

			vbPos += batchRenderObjects_( pass, vbPos );
		}

		return vbPos;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::makeBatches_( bool & _overflow )
	{		
		//m_vbPos = 0;

		size_t vbPos = m_vbPos;
		vbPos = this->makeBatch_( vbPos );

		size_t vertexDataSize = vbPos - m_vbPos;

		if( vertexDataSize == 0 )	// nothing to render
		{
			return true;
		}

		if( vertexDataSize > m_maxPrimitiveVertices2D )
		{
			_overflow = true;
			return true;
		}

		if( vertexDataSize > m_maxPrimitiveVertices2D - m_vbPos )
		{
			m_vbPos = 0;

			vbPos = this->makeBatch_( 0 );

			vertexDataSize = vbPos;
		}

		uint32 lockFlags = m_vbPos ? LOCK_NOOVERWRITE : LOCK_DISCARD;

		void * vData = m_interface->lockVertexBuffer( 
			m_vbHandle2D,
			m_vbPos * sizeof(Vertex2D),
			vertexDataSize * sizeof(Vertex2D), 
			lockFlags 
			);

		if( vData == NULL )
		{
			MENGE_LOG_ERROR("Critical error: failed to lock vertex buffer");

			return false;
		}

		size_t offset = 0;
		
		Vertex2D * vertexBuffer = (Vertex2D *)vData;

		for( TVectorRenderPass::iterator 
			it = m_passes.begin(), 
			it_end = m_passes.end();
			it != it_end;
			++it )
		{
			RenderPass * pass = (*it);
			
			offset = this->insertRenderObjects_( pass, vertexBuffer, offset );
		}

		if( m_interface->unlockVertexBuffer( m_vbHandle2D ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to unlock vertex buffer" );
			return false;
		}

		_overflow = false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_checkForBatch( RenderObject* _prev, RenderObject* _next )
	{
		if( _prev->primitiveType == PT_LINESTRIP		// this primitives could'n be batched
			|| _prev->primitiveType == PT_TRIANGLESTRIP 
			|| _prev->primitiveType == PT_TRIANGLEFAN )
		{
			return false;
		}

		if( _prev->textureStages != _next->textureStages )
		{
			return false;
		}

		for( size_t i = 0; i != _prev->textureStages; ++i )
		{
			RenderImageInterface * prevImage = _prev->textures[i]->getImage();
			RenderImageInterface * nextImage = _next->textures[i]->getImage();

			if( prevImage != nextImage )
			{
				return false;
			}
		}

		if( _prev->material != _next->material )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderEngine::batchRenderObjects_( RenderPass * _pass, size_t _startVertexPos )
	{
		RenderObject* batchedRO = NULL;
		size_t verticesNum = _startVertexPos;

		TVectorRenderObject & renderObjects = _pass->renderObjects;

		for( TVectorRenderObject::iterator 
			it = renderObjects.begin(), 
			it_end = renderObjects.end();
		it != it_end;
		++it )
		{
			RenderObject * renderObject = &(*it);

			if( this->batchRenderObject_( renderObject, batchedRO, verticesNum ) == false )
			{
				batchedRO = renderObject;
			}
		}

		return (verticesNum - _startVertexPos);
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::batchRenderObject_( RenderObject * _renderObject, RenderObject * _batchedObject, size_t & _verticesNum ) const
	{
		size_t vertexStride = m_primitiveVertexStride[_renderObject->logicPrimitiveType];
		size_t align = (vertexStride - (_verticesNum % vertexStride)) % vertexStride;
		_verticesNum += align + _renderObject->verticesNum;

		if( _batchedObject != NULL && s_checkForBatch( _batchedObject, _renderObject ) == true )
		{
			_batchedObject->dipIndiciesNum += _renderObject->dipIndiciesNum;
			_batchedObject->dipVerticesNum += _renderObject->verticesNum;
			_renderObject->dipVerticesNum = 0;
			_renderObject->dipIndiciesNum = 0;

			_renderObject->baseVertexIndex = 0;

			return true;
		}
		
		_renderObject->dipVerticesNum = _renderObject->verticesNum;
		_renderObject->baseVertexIndex = 0;
				
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderEngine::insertRenderObjects_( RenderPass * _pass, Vertex2D * _vertexBuffer, size_t _offset )
	{
		TVectorRenderObject & renderObjects = _pass->renderObjects;

		for( TVectorRenderObject::iterator
			it = renderObjects.begin(), 
			it_end = renderObjects.end();
		it != it_end;
		++it )
		{
			RenderObject* ro = &(*it);

			_offset = this->insertRenderObject_( ro, _vertexBuffer, _offset );			
		}

		return _offset;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderEngine::insertRenderObject_( RenderObject * _renderObject, Vertex2D * _vertexBuffer, size_t _offset ) const
	{
		ELogicPrimitiveType type = _renderObject->logicPrimitiveType;

		size_t indexStart = m_primitiveIndexStart[type];
		size_t indexStride = m_primitiveIndexStride[type];
		size_t vertexStride = m_primitiveVertexStride[type];

		size_t align = ( vertexStride - ( m_vbPos % vertexStride ) ) % vertexStride;

		_offset += align;
		m_vbPos += align;

		if( m_vbPos + _renderObject->verticesNum > m_maxPrimitiveVertices2D )
		{
			MENGE_LOG_ERROR("Warning: vertex buffer overflow");
			_renderObject->verticesNum = m_maxPrimitiveVertices2D - m_vbPos;
			//return;
		}

		_renderObject->startIndex  = indexStart + m_vbPos / vertexStride * indexStride;
		_renderObject->minIndex = (_renderObject->startIndex - indexStart) / indexStride * vertexStride;

		assert( _renderObject->startIndex + _renderObject->dipIndiciesNum <= m_maxIndexCount );

		//Vertex2D * vertexBuffer = _vertexBuffer + _offset * sizeof(Vertex2D)
		Vertex2D * offsetVertexBuffer = _vertexBuffer + _offset;
		std::copy( _renderObject->vertexData, _renderObject->vertexData + _renderObject->verticesNum, offsetVertexBuffer );

		m_vbPos += _renderObject->verticesNum;
		_offset += _renderObject->verticesNum;

		//m_logicPrimitiveCount[type] += 1;

		return _offset;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::flushRender_()
	{
		bool overflow = false;
		if( this->makeBatches_( overflow ) == false )
		{
			return;
		}

		if( overflow == true )
		{
			m_vbPos = 0;

			MENGE_LOG_WARNING("Warning: vertex buffer overflow");
			////vertexDataSize = m_maxPrimitiveVertices2D;
			//setRenderSystemDefaults_( m_maxQuadCount * 2 );
			//recreate2DBuffers_( m_maxIndexCount );
			//restoreRenderSystemStates_();
			//prepare2D_();
			return;
		}

		this->render_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::prepare2D_()
	{
		if( m_currentVBHandle != m_vbHandle2D )
		{
			m_currentVBHandle = m_vbHandle2D;
			m_interface->setVertexBuffer( m_currentVBHandle );
		}

		if( m_currentVertexDeclaration != Vertex2D_declaration )
		{
			m_currentVertexDeclaration = Vertex2D_declaration;
			m_interface->setVertexDeclaration( sizeof(Vertex2D), m_currentVertexDeclaration );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::prepare3D_()
	{		
		if( m_currentVertexDeclaration != Vertex3D_declaration )
		{
			m_currentVertexDeclaration = Vertex3D_declaration;
			m_interface->setVertexDeclaration( sizeof(Vertex3D), m_currentVertexDeclaration );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseRenderPass_( RenderPass* _pass )
	{
		_pass->renderObjects.clear();

		m_poolRenderPass.release( _pass );
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderEngine::refillIndexBuffer2D_()
	{
		uint16* ibuffer = m_interface->lockIndexBuffer( m_ibHandle2D );

		// s
		uint16 vertexCount = 0;
		uint16 maxVertices = 0;
		for( size_t i = m_primitiveIndexStart[LPT_QUAD];
			i + m_primitiveIndexStride[LPT_QUAD] < m_primitiveIndexStart[LPT_LINE];
			i += 6, vertexCount += 4 )
		{
			ibuffer[i+0] = 0 + vertexCount;
			ibuffer[i+1] = 3 + vertexCount;
			ibuffer[i+2] = 1 + vertexCount;
			ibuffer[i+3] = 1 + vertexCount;
			ibuffer[i+4] = 3 + vertexCount;
			ibuffer[i+5] = 2 + vertexCount;
		}
		maxVertices = vertexCount;
		// TRIANGLES
		//vertexCount = 0;
		//for( size_t i = m_primitiveIndexStart[LPT_TRIANGLE];
		//	i + m_primitiveIndexStride[LPT_TRIANGLE] < m_primitiveIndexStart[LPT_LINE];
		//	i += 3, vertexCount += 3 )
		//{
		//	ibuffer[i+0] = 0 + vertexCount;
		//	ibuffer[i+1] = 1 + vertexCount;
		//	ibuffer[i+2] = 2 + vertexCount;
		//}
		maxVertices = std::min( maxVertices, vertexCount );
		// LINES
		vertexCount = 0;
		for( size_t i = m_primitiveIndexStart[LPT_LINE];
			i + m_primitiveIndexStride[LPT_LINE] < m_maxIndexCount;
			i += 1, vertexCount += 1 )
		{
			ibuffer[i+0] = 0 + vertexCount;
		}
		maxVertices = std::min( maxVertices, vertexCount );
		// RECTANGLES
		//vertexCount = 0;
		//for( size_t i = m_primitiveIndexStart[LPT_RECTANGLE];
		//	i + m_primitiveIndexStride[LPT_RECTANGLE] < m_maxIndexCount;
		//	i += 5, vertexCount += 4 )
		//{
		//	ibuffer[i+0] = 0 + vertexCount;
		//	ibuffer[i+1] = 1 + vertexCount;
		//	ibuffer[i+2] = 2 + vertexCount;
		//	ibuffer[i+3] = 3 + vertexCount;
		//	ibuffer[i+4] = 0 + vertexCount;
		//}

		maxVertices = std::min( maxVertices, vertexCount );

		if( m_interface->unlockIndexBuffer( m_ibHandle2D ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to unlock index buffer" );
		}
		return maxVertices;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::recreate2DBuffers_( size_t _maxIndexCount )
	{
		if( m_ibHandle2D != 0 )
		{
			m_interface->releaseIndexBuffer( m_ibHandle2D );
			if( m_currentIBHandle == m_ibHandle2D )
			{
				m_currentIBHandle = 0;
			}
			m_ibHandle2D = 0;
		}

		m_ibHandle2D = m_interface->createIndexBuffer( _maxIndexCount );
		if( m_ibHandle2D == 0 )
		{
			MENGE_LOG_ERROR( "Critical error: can't create index buffer for %d indicies"
				, m_maxIndexCount 
				);

			return false;
		}

		m_maxPrimitiveVertices2D = refillIndexBuffer2D_();

		if( m_vbHandle2D != 0 )
		{
			m_interface->releaseVertexBuffer( m_vbHandle2D );
			if( m_currentVBHandle == m_vbHandle2D )
			{
				m_currentVBHandle = 0;
			}
			m_vbHandle2D = 0;
		}

		m_vbHandle2D = m_interface->createVertexBuffer( m_maxPrimitiveVertices2D, sizeof(Vertex2D) );
		if( m_vbHandle2D == 0 )
		{
			MENGE_LOG_ERROR( "Critical error: can't create index buffer for %d indicies"
				, m_maxIndexCount 
				);

			return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderSystemDefaults_( size_t _maxQuadCount )
	{
		m_primitiveCount[LPT_QUAD] = _maxQuadCount;

		m_primitiveIndexStride[LPT_QUAD] = 6;
		m_primitiveVertexStride[LPT_QUAD] = 4;
		//m_primitiveIndexStride[LPT_TRIANGLE] = 3;
		//m_primitiveVertexStride[LPT_TRIANGLE] = 3;
		m_primitiveIndexStride[LPT_LINE] = 1;
		m_primitiveVertexStride[LPT_LINE] = 1;
		//m_primitiveIndexStride[LPT_RECTANGLE] = 5;
		//m_primitiveVertexStride[LPT_RECTANGLE] = 4;

		for( size_t i = 1; i != LPT_PRIMITIVE_COUNT; ++i )
		{
			m_primitiveCount[i] = m_primitiveCount[LPT_QUAD] * m_primitiveVertexStride[LPT_QUAD] / m_primitiveVertexStride[i];
		}

		m_primitiveIndexStart[LPT_QUAD] = 0;
		for( size_t i = 0; i != LPT_PRIMITIVE_COUNT - 1; ++i )
		{
			m_primitiveIndexStart[i+1] = m_primitiveIndexStart[i] + m_primitiveCount[i] * m_primitiveIndexStride[i];
		}

		for( size_t i = 0; i != LPT_PRIMITIVE_COUNT; ++i )
		{
			m_maxIndexCount += m_primitiveCount[i] * m_primitiveIndexStride[i];
		}

		m_currentBlendSrc = BF_ONE;
		m_currentBlendDst = BF_ZERO;
		m_depthBufferWriteEnable = false;
		m_alphaBlendEnable = false;
		m_alphaTestEnable = false;

		const mt::mat4f * matrix_zero_ptr = 0;
		std::fill_n( m_currentMatrixUV, MENGE_MAX_TEXTURE_STAGES, matrix_zero_ptr);
		std::fill_n( m_currentMaskUV, MENGE_MAX_TEXTURE_STAGES, matrix_zero_ptr);

		std::fill_n( m_currentTexturesID, MENGE_MAX_TEXTURE_STAGES, 0 );

		m_poolRenderPass.initialize(25);
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::enableTextureFiltering( bool _enable )
	{
		m_textureFiltering = _enable;
		
		if( m_interface == NULL )
		{
			return;
		}

		//for( int i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
		//{
		//	ETextureFilter tFilter = m_textureFiltering ? TF_LINEAR : TF_NONE;

		//	m_interface->setTextureStageFilter( i, TFT_MIPMAP, TF_NONE );
		//	m_interface->setTextureStageFilter( i, TFT_MAGNIFICATION, tFilter );
		//	m_interface->setTextureStageFilter( i, TFT_MINIFICATION, tFilter );
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::isTextureFilteringEnabled() const
	{
		return m_textureFiltering;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::isResolutionAppropriate() const
	{
		return m_renderTargetResolution == m_contentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setVSync( bool _vSync )
	{
		if( m_vsync == _vSync )
		{
			return;
		}

		m_vsync = _vSync;
		if( m_windowCreated == true )
		{
			m_interface->setVSync( m_vsync );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::getVSync() const
	{
		return m_vsync;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::supportA8() const
	{
		return m_supportA8;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::clear( uint32 _color )
	{
		m_interface->clear( _color );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderTargetTexture( RenderTextureInterface * _texture, bool _clear )
	{
		RenderImageInterface * image = _texture->getImage();
		
		m_interface->setRenderTarget( image, _clear );

		this->restoreRenderSystemStates_();
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface * RenderEngine::createRenderTargetTexture( size_t _width, size_t _height, PixelFormat _format )
	{
		MENGE_LOG_INFO( "Creating texture %dx%d %d"
			, _width
			, _height
			, _format 
			);

		size_t hwWidth;
		size_t hwHeight;
		PixelFormat hwFormat = _format;

		RenderImageInterface* image = NULL;
		image = m_interface->createRenderTargetImage( _width, _height, hwWidth, hwHeight, hwFormat );

		if( image == NULL )
		{
			MENGE_LOG_ERROR( "Error: (RenderEngine::createRenderTargetImage) RenderSystem couldn't create RenderTargetTexture %dx%d"
				, _width
				, _height 
				);

			return NULL;
		}

		size_t memroy_size = PixelUtil::getMemorySize( hwWidth, hwHeight, 1, hwFormat );
		m_debugInfo.textureMemory += memroy_size;
		++m_debugInfo.textureCount;

		//printf("m_debugInfo.textureMemory %d %f\n", m_debugInfo.textureCount, float(m_debugInfo.textureMemory) / (1024.f * 1024.f));

		RenderTexture* texture = new RenderTexture( image, _width, _height, _format, hwWidth, hwHeight, hwFormat, ++m_idEnumerator );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setSeparateAlphaBlendMode()
	{
		m_interface->setSeparateAlphaBlendMode();
	}
	//////////////////////////////////////////////////////////////////////////
}
