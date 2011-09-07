#	include "RenderEngine.h"

#	include "FileEngine.h"

#	include "LogEngine.h"

#	include "Application.h"
#	include "Game.h"

#	include "CodecEngine.h"
#	include "Interface/ImageCodecInterface.h"

//#	include "ResourceTexture.h"
#	include "ResourceImage.h"
#	include "Material.h"

#	include "Camera.h"

#	include "Texture.h"
#	include "Vertex.h"

#	include "Core/PixelFormat.h"

#	include "Consts.h"

#	include <algorithm>
#	include <map>
#	include <ctime>

namespace Menge
{
	struct RenderObject
	{
		const Material * material;

		std::size_t textureStages;
		const Texture* textures[MENGE_MAX_TEXTURE_STAGES];

		mt::mat4f * matrixUV[MENGE_MAX_TEXTURE_STAGES];

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

	typedef std::vector<RenderObject> TVectorRenderObject;

	struct RenderPass
	{
		TVectorRenderObject renderObjects;

		Viewport renderport;
		mt::mat3f wm;
	};
	////////////////////////////////////////////////////////////////////////////
	//static bool s_checkForBatch( RenderObject* _prev, RenderObject* _next )
	//{
	//	if( _prev == NULL || _next == NULL )
	//	{
	//		return false;
	//	}

	//	if( _next->logicPrimitiveType == LPT_MESH )
	//	{
	//		return false;
	//	}

	//	if( _prev->primitiveType == PT_LINESTRIP		// this primitives could'n be batched
	//		|| _prev->primitiveType == PT_TRIANGLESTRIP 
	//		|| _prev->primitiveType == PT_TRIANGLEFAN 
	//		|| _prev->textureStages != _next->textureStages )
	//	{
	//		return false;
	//	}

	//	for( std::size_t i = 0; i != _prev->textureStages; ++i )
	//	{
	//		if( _prev->textures[i] != _next->textures[i] )
	//		{
	//			return false;
	//		}
	//	}

	//	if( _prev->material != _next->material )
	//	{
	//		return false;
	//	}

	//	return true;
	//}
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
		, m_camera(0)
		, m_dipCount(0)
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
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::initialize( size_t _maxQuadCount )
	{
		bool result = initInterfaceSystem( &m_interface );
		if( ( result == false ) || ( m_interface == NULL ) )
		{
			return false;
		}

		setRenderSystemDefaults_( _maxQuadCount );

		LogSystemInterface * system = LogEngine::get()->getInterface();

		if( m_interface->initialize( system, this ) == false )
		{
			return false;
		}
		
		{
			Material mt;

			mt.alphaBlendEnable = true;
			mt.alphaTestEnable = false;
			mt.depthBufferWriteEnable = false;

			mt.blendSrc = BF_SOURCE_ALPHA;
			mt.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;
			mt.textureStage[0].alphaOp = TOP_MODULATE;
			mt.textureStage[0].colorOp = TOP_MODULATE;

			this->createMaterialGroup( CONST_STRING(BlendSprite), mt );
		}

		{
			Material mt;

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
			Material mt;

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
			Material mt;

			mt.alphaBlendEnable = true;
			mt.alphaTestEnable = true;
			mt.depthBufferWriteEnable = false;

			mt.blendSrc = BF_SOURCE_ALPHA;
			mt.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

			TextureStage & ts0 = mt.textureStage[0];

			ts0.colorOp = TOP_MODULATE;
			ts0.colorArg1 = TARG_TEXTURE;
			ts0.colorArg2 = TARG_DIFFUSE;
			ts0.alphaOp = TOP_SELECTARG1;
			ts0.alphaArg1 = TARG_DIFFUSE;

			TextureStage & ts1 = mt.textureStage[1];

			ts1.colorOp = TOP_SELECTARG1;
			ts1.colorArg1 = TARG_CURRENT;
			ts1.alphaOp = TOP_MODULATE;
			ts1.alphaArg1 = TARG_TEXTURE;
			ts1.alphaArg2 = TARG_CURRENT;

			this->createMaterialGroup( CONST_STRING(ExternalAlpha), mt );
		}

		{
			Material mt;

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
			Material mt;

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
			Material mt;

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
			Material mt;

			mt.alphaBlendEnable = true;
			mt.alphaTestEnable = false;
			mt.depthBufferWriteEnable = false;

			mt.blendSrc = BF_SOURCE_ALPHA;
			mt.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

			mt.textureStage[0].colorOp = TOP_MODULATE;
			mt.textureStage[0].alphaOp = TOP_MODULATE;

			this->createMaterialGroup( CONST_STRING(ParticleBlend), mt );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _viewport, int _bits, bool _fullscreen,
		WindowHandle _winHandle, int _FSAAType, int _FSAAQuality )
	{
		m_windowResolution = _resolution;
		m_contentResolution = _contentResolution;
		m_viewport = _viewport;

		m_fullscreen = _fullscreen;
		
		std::size_t width = m_windowResolution.getWidth();
		std::size_t height = m_windowResolution.getHeight();

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

		m_nullTexture = createTexture( Consts::get()->c_NullTexture, 2, 2, PF_R8G8B8 );

		if( m_nullTexture )
		{
			int pitch = 0;
			unsigned char* textureData = m_nullTexture->lock( &pitch, false );
			std::fill( textureData, textureData + pitch * 2, 0xFF );
			m_nullTexture->unlock();
		}

		if( this->recreate2DBuffers_( m_maxIndexCount ) == false )
		{
			return false;
		}

		restoreRenderSystemStates_();
		prepare2D_();

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
	void RenderEngine::screenshot( Texture* _image, const mt::vec4f & _rect )
	{
		RenderImageInterface* iInterface = _image->getInterface();

		m_interface->screenshot( iInterface, _rect.buff() );
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::beginLayer2D()
	{
		//if( m_layer3D == true )
		//{
		//	prepare3D_();
		//	m_interface->clearFrameBuffer( FBT_DEPTH );
		//	m_layer3D = false;
		//	flushRender_();
		//	m_interface->beginLayer2D();
		//}
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::endLayer2D()
	{
		m_interface->endLayer2D();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::beginLayer3D()
	{
		/*if( m_layer3D == false )
		{
			prepare2D_();
			m_interface->clearFrameBuffer( FBT_DEPTH );
			m_layer3D = true;
			flushRender_();
			m_interface->beginLayer3D();
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	//void RenderEngine::setProjectionMatrix( const mt::mat4f& _projection )
	//{
	//	RenderObject* ro = getTempRenderObject_();
	//	ro->setProjTransform = true;
	//	ro->projTransform = _projection;
	//	renderObject( ro );
	//}
	//////////////////////////////////////////////////////////////////////////
	//void RenderEngine::setViewMatrix( const mt::mat4f& _view )
	//{
	//	RenderObject* ro = getTempRenderObject_();
	//	ro->setViewTransform = true;
	//	ro->viewTransform = _view;
	//	renderObject( ro );
	//}
	//////////////////////////////////////////////////////////////////////////
	//void RenderEngine::setWorldMatrix( const mt::mat4f& _world )
	//{
	//	RenderObject* ro = getTempRenderObject_();
	//	ro->setWorldTransform = true;
	//	ro->worldTransform = _world;
	//	renderObject( ro );
	//}	
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createMaterialGroup( const ConstString & _name, const Material & _material )
	{
		TMapMaterialGroup::iterator it_found = m_mapMaterialGroup.find( _name );

		if( it_found != m_mapMaterialGroup.end() )
		{
			MENGE_LOG_ERROR("Error: MaterialGroup '%s' is already created!"
				, _name.c_str()
				);

			return false;
		}

		MaterialGroup * materialGroup = new MaterialGroup;

		materialGroup->group[0] = _material;
		materialGroup->group[0].textureStage[0].addressU = TAM_CLAMP;
		materialGroup->group[0].textureStage[0].addressV = TAM_CLAMP;

		materialGroup->group[1] = _material;
		materialGroup->group[1].textureStage[0].addressU = TAM_WRAP;
		materialGroup->group[1].textureStage[0].addressV = TAM_CLAMP;

		materialGroup->group[2] = _material;
		materialGroup->group[2].textureStage[0].addressU = TAM_CLAMP;
		materialGroup->group[2].textureStage[0].addressV = TAM_WRAP;

		materialGroup->group[2] = _material;
		materialGroup->group[2].textureStage[0].addressU = TAM_WRAP;
		materialGroup->group[2].textureStage[0].addressV = TAM_WRAP;

		m_mapMaterialGroup.insert( std::make_pair(_name, materialGroup) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const MaterialGroup * RenderEngine::getMaterialGroup( const ConstString & _name )
	{
		TMapMaterialGroup::iterator it_found = m_mapMaterialGroup.find( _name );

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
	Texture* RenderEngine::createTexture( const ConstString & _name, size_t _width, size_t _height, PixelFormat _format )
	{
		TMapTextures::iterator it_find = m_textures.find( _name );

		if( it_find != m_textures.end() )
		{
			MENGE_LOG_WARNING( "Warning: (RenderEngine::createImage) Image '%s' already exist"
				, _name.c_str()
				);

			return 0;
		}

		Texture * texture = createTexture_( _name, _width, _height, _format );

		if( texture == 0 )
		{
			return 0;
		}

		m_textures.insert( std::make_pair( _name, texture ) );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* RenderEngine::createTexture_( const ConstString & _name, size_t _width, size_t _height, PixelFormat _format )
	{
		MENGE_LOG_INFO( "Creating texture '%s' %dx%d %d"
			, _name.c_str()
			, _width
			, _height
			, _format 
			);

		size_t hwWidth;
		size_t hwHeight;

		RenderImageInterface* image = m_interface->createImage( _width, _height, hwWidth, hwHeight, _format );

		if( image == NULL )
		{
			MENGE_LOG_ERROR( "Error: (RenderEngine::createImage) RenderSystem couldn't create image '%s' %dx%d"
				, _name.c_str()
				, _width
				, _height 
				);

			return NULL;
		}

		std::size_t memroy_size = PixelUtil::getMemorySize( hwWidth, hwHeight, 1, _format );
		m_debugInfo.textureMemory += memroy_size;
		++m_debugInfo.textureCount;

		Texture* texture = new Texture( image, _name, _width, _height, _format, hwWidth, hwHeight, _format, ++m_idEnumerator );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::saveImage( Texture* _image, const ConstString & _fileSystemName, const String & _filename )
	{
		FileOutputInterface * stream = FileEngine::get()
			->openOutputFile( _fileSystemName, _filename );

		if( stream == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : can't create file '%s'"
				, _filename.c_str() 
				);

			return false;
		}

		ImageEncoderInterface * imageEncoder = CodecEngine::get()
			->createEncoderT<ImageEncoderInterface>( Consts::get()->c_Image, stream );

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
		unsigned char* buffer = _image->lock( &pitch );

		/*unsigned char* lockBuffer = new unsigned char[ imgData.height * pitch ];

		std::size_t mPitch = Menge::PixelUtil::getNumElemBytes( imgData.format ) * imgData.width;
		for( std::size_t i = 0; i != imgData.height; i++ )
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

		imageEncoder->release();

		if( bytesWritten == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::saveImage : Error while encoding image data" );
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* RenderEngine::loadTexture( const ConstString& _pakName, const ConstString & _filename, const ConstString& _codec )
	{
		TMapTextures::iterator it_find = m_textures.find( _filename );
		
		if( it_find != m_textures.end() )
		{
			it_find->second->addRef();
			return it_find->second;
		}
		
		FileInputInterface * stream = FileEngine::get()
			->openInputFile( _pakName, Helper::to_str(_filename) );

		if( stream == 0 )
		{
			MENGE_LOG_ERROR( "Warning: Image file '%s' was not found"
				, _filename.c_str() 
				);

			return NULL;
		}

		ImageDecoderInterface * imageDecoder = CodecEngine::get()
			->createDecoderT<ImageDecoderInterface>( _codec, stream );

		if( imageDecoder == 0 )
		{
			MENGE_LOG_ERROR( "Warning: Image decoder for file '%s' was not found"
				, _filename.c_str() 
				);

			stream->close();

			return NULL;
		}

		const ImageCodecDataInfo* dataInfo = imageDecoder->getCodecDataInfo();

		if( dataInfo->format == PF_UNKNOWN )
		{
			MENGE_LOG_ERROR( "Error: Invalid image format '%s'"
				, _filename.c_str() 
				);

			imageDecoder->destroy();

			stream->close();

			return NULL;
		}

		Texture * texture = createTexture_( _filename, dataInfo->width, dataInfo->height, dataInfo->format );

		if( texture == NULL )
		{
			imageDecoder->destroy();

			stream->close();

			return NULL;
		}

		texture->loadImageData( imageDecoder );

		imageDecoder->destroy();

		stream->close();

		m_textures.insert( std::make_pair( _filename, texture ) );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseTexture( const Texture* _texture )
	{
		if( _texture == NULL )
		{
			return;
		}

		if( _texture->decRef() == 0 )
		{
			const ConstString & name = _texture->getName();
			m_textures.erase( name );

			this->destroyTexture_( _texture );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::destroyTexture_( const Texture* _texture )
	{
		RenderImageInterface* image = _texture->getInterface();

		size_t width = _texture->getWidth();
		size_t height = _texture->getHeight();

		PixelFormat format = _texture->getPixelFormat();

		std::size_t memroy_size = PixelUtil::getMemorySize( width, height, 1, format );
		m_debugInfo.textureMemory -= memroy_size;
		--m_debugInfo.textureCount;

		m_interface->releaseImage( image );

		delete _texture;
	}
	//////////////////////////////////////////////////////////////////////////
	LightInterface * RenderEngine::createLight( const ConstString& _name )
	{
		return m_interface->createLight( Helper::to_str(_name) );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseLight( LightInterface * _light )
	{
		return m_interface->releaseLight( _light );
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

		std::size_t count = resolutions.size() / 2;

		for( std::size_t i = 0; i < count; i++ )
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
			std::size_t height = _resolution.getHeight();
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
		refillIndexBuffer2D_();
		restoreRenderSystemStates_();
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
		m_passes.clear();

		m_currentPass = NULL;

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
		flushRender_();
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
	void RenderEngine::applyRenderViewport( const Viewport & _viewport )
	{
		//printf("_viewport %f:%f - %f:%f\n", _viewport.begin.x, _viewport.begin.y, _viewport.end.x, _viewport.end.y);

		//mt::vec2f scale = m_renderTargetResolution.getScale(m_contentResolution);

		//float vpcx = _viewport.begin.x + _viewport.getWidth() * 0.5f;
		//float vpcy = _viewport.begin.y + _viewport.getHeight() * 0.5f;

		float wscale = _viewport.getWidth();
		float hscale = _viewport.getHeight();

		Viewport rv;
		rv.begin.x = m_renderOffset.x + _viewport.begin.x * m_renderScale.x + 0.5;
		rv.begin.y = m_renderOffset.y + _viewport.begin.y * m_renderScale.y + 0.5;

		rv.end.x = m_renderOffset.x + _viewport.end.x * m_renderScale.x;
		rv.end.y = m_renderOffset.y + _viewport.end.y * m_renderScale.y;

		m_interface->setRenderViewport( rv );
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
	void RenderEngine::newRenderPass( const Viewport & _viewport, const mt::mat3f & _wm )
	{
		RenderPass * pass = this->createRenderPass_();

		pass->renderport = _viewport;
		pass->wm = _wm;

		this->setRenderPass(pass);
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::getCurrentRenderPass( Viewport & _viewport, mt::mat3f & _wm ) const
	{
		if( m_currentPass == 0 )
		{
			return false;
		}

		_viewport = m_currentPass->renderport;
		_wm = m_currentPass->wm;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::isWindowCreated() const
	{
		return m_windowCreated;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderPass_( RenderObject* _renderObject )
	{
		if( m_currentTextureStages > _renderObject->textureStages )
		{
			for( std::size_t stageId = _renderObject->textureStages; stageId != m_currentTextureStages; ++stageId )
			{
				disableTextureStage_( stageId );
			}
		}

		const Material* material = _renderObject->material;

		m_currentTextureStages = _renderObject->textureStages;

		for( std::size_t stageId = 0; stageId != m_currentTextureStages; ++stageId )
		{
			const Texture * texture = _renderObject->textures[stageId];

			if( texture == NULL )
			{
				texture = m_nullTexture;
			}
		
			if( texture->getID() != m_currentTexturesID[stageId] || m_currentTexturesID[stageId] != 0 )
			{
				m_currentTexturesID[stageId] = texture->getID();
				RenderImageInterface* t = texture->getInterface();

				m_interface->setTexture( stageId, t );
			}

			if( m_currentMaterial != material )
			{
				TextureStage & current_stage = m_currentTextureStage[stageId];
				const TextureStage & stage = material->textureStage[stageId];

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
			0, 
			_renderObject->minIndex,
			_renderObject->dipVerticesNum, 
			_renderObject->startIndex, 
			_renderObject->dipIndiciesNum 
			);

		m_currentMaterial = material;

		++m_dipCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::disableTextureStage_( std::size_t _stage )
	{
		TextureStage & stage = m_currentTextureStage[_stage];

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
		//mt::mat4f worldTransform;

		mt::ident_m4( viewTransform );
		mt::ident_m4( projTransform );
		//mt::ident_m4( worldTransform );

		m_interface->setVertexBuffer( m_currentVBHandle );
		m_interface->setIndexBuffer( m_currentIBHandle, m_currentBaseVertexIndex );
		m_interface->setVertexDeclaration( Vertex2D_declaration );
		m_interface->setProjectionMatrix( projTransform );
		m_interface->setModelViewMatrix( viewTransform );
		//m_interface->setWorldMatrix( worldTransform );
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
			TextureStage & stage = m_currentTextureStage[i];

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

			ETextureFilter tFilter = m_textureFiltering ? TF_LINEAR : TF_NONE;

			m_interface->setTextureStageFilter( i, TFT_MIPMAP, TF_NONE );
			m_interface->setTextureStageFilter( i, TFT_MAGNIFICATION, tFilter );
			m_interface->setTextureStageFilter( i, TFT_MINIFICATION, tFilter );
			
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
	void RenderEngine::setProjectionMatrix2D_( mt::mat4f& _out, float l, float r, float b, float t, float zn, float zf )
	{
		m_interface->makeProjection2D( l, r, t, b, zn, zf, &(_out.v0[0]) );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setCamera( Camera * _camera )
	{
		m_camera = _camera;
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
	void RenderEngine::setRenderPass( RenderPass * _pass )
	{
		m_currentPass = _pass;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderPass * RenderEngine::getRenderPass() const
	{
		return m_currentPass;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderEngine::DebugInfo& RenderEngine::getDebugInfo() const
	{
		return m_debugInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::hasTexture( const ConstString & _name )
	{
		TMapTextures::iterator it_find = m_textures.find( _name );

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
		if( m_camera == 0 )
		{
			return;
		}

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

			const ConstString& renderTarget = m_camera->getRenderTarget();

			if( renderTarget != m_currentRenderTarget )
			{
				m_currentRenderTarget = renderTarget;
				m_interface->setRenderTarget( NULL, true );
				m_renderTargetResolution = m_windowResolution;
			}

			this->applyRenderViewport( renderPass->renderport );

			TVectorRenderObject & renderObjects = renderPass->renderObjects;

			for( TVectorRenderObject::iterator 
				it = renderObjects.begin(), 
				it_end = renderObjects.end();
			it != it_end; 
			++it )
			{
				RenderObject* renderObject = &(*it);

				if( renderObject->dipIndiciesNum == 0 
					|| renderObject->dipVerticesNum == 0 )
				{
					continue;
				}

				this->renderPass_( renderObject );
			}

			releaseRenderPass_( *rit );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//FIXME: _vertices меняеться внутри рендера, поэтому нельзя рендерить несколько нод в разных местах :(
	//FIXED! уже можно!
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderObject2D( const Material* _material, const Texture** _textures, mt::mat4f * const * _matrixUV, int _texturesNum,
										const Vertex2D* _vertices, size_t _verticesNum,
										ELogicPrimitiveType _type, size_t _indicesNum, IBHandle _ibHandle )
	{
		if( m_currentPass == 0 )
		{
			return;
		}

		RenderObject renderObject;
		RenderObject* ro = &renderObject;

		ro->material = _material;
		ro->textureStages = _texturesNum;

		ro->logicPrimitiveType = _type;

		switch( _type )
		{
		case LPT_MESH:
			ro->dipIndiciesNum = _indicesNum;
			ro->ibHandle = _ibHandle;
			break;
		default:
			ro->dipIndiciesNum = m_primitiveIndexStride[_type] * _verticesNum / m_primitiveVertexStride[_type];
			ro->ibHandle = m_ibHandle2D;
			break;
		}
	
		ro->vertexData = _vertices;
		ro->verticesNum = _verticesNum;

		ro->baseVertexIndex = 0;

		for( int i = 0; i != _texturesNum; ++i )
		{
			if( _textures == NULL )
			{
				ro->textures[i] = 0;
			}
			else
			{
				ro->textures[i] = _textures[i];
			}
			
			ro->matrixUV[i] = _matrixUV == NULL ? NULL : _matrixUV[i];
		}

		switch( _type )
		{
		case LPT_QUAD:
		case LPT_TRIANGLE:
			ro->primitiveType = PT_TRIANGLELIST;
			break;
		case LPT_LINE:
		case LPT_RECTANGLE:
			ro->primitiveType = PT_LINESTRIP;
			break;
		case LPT_MESH:
			ro->primitiveType = PT_TRIANGLESTRIP;
			break;
		default:
			ro->primitiveType = PT_TRIANGLELIST;
		}

		m_currentPass->renderObjects.push_back( renderObject );
	}
	//////////////////////////////////////////////////////////////////////////
	VBHandle RenderEngine::createVertexBuffer( const Vertex2D * _buffer, std::size_t _count )
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
	IBHandle RenderEngine::createIndicesBuffer( const unsigned short * _buffer, std::size_t _count )
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
	void RenderEngine::updateVertexBuffer( VBHandle _handle, const Vertex2D * _buffer, std::size_t _count  )
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
	void RenderEngine::updateIndicesBuffer( IBHandle _handle, const unsigned short * _buffer, std::size_t _count )
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
		unsigned char* vData = (unsigned char*)m_interface->lockVertexBuffer( 
			m_vbHandle2D,
			m_vbPos * sizeof(Vertex2D),
			vertexDataSize * sizeof(Vertex2D), 
			lockFlags );

		if( vData == NULL )
		{
			MENGE_LOG_ERROR("Critical error: failed to lock vertex buffer");
			return false;
		}

		size_t offset = 0;
		
		for( TVectorRenderPass::iterator 
			it = m_passes.begin(), 
			it_end = m_passes.end();
			it != it_end;
			++it )
		{
			RenderPass * pass = (*it);

			Vertex2D * vertexBuffer = (Vertex2D *)vData;

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
	size_t RenderEngine::batchRenderObjects_( RenderPass * _pass, size_t _startVertexPos )
	{
		//RenderObject* batchedRO = NULL;
		size_t verticesNum = _startVertexPos;

		TVectorRenderObject & renderObjects = _pass->renderObjects;

		for( TVectorRenderObject::iterator 
			it = renderObjects.begin(), 
			it_end = renderObjects.end();
		it != it_end;
		++it )
		{
			RenderObject * renderObject = &(*it);

			switch( renderObject->logicPrimitiveType )
			{
			case LPT_MESH:
				{
					//batchedRO = renderObject;
					renderObject->dipVerticesNum = renderObject->verticesNum;
					renderObject->baseVertexIndex = verticesNum;

					verticesNum += renderObject->verticesNum;
				}break;
			default:
				{
					size_t vertexStride = m_primitiveVertexStride[renderObject->logicPrimitiveType];
					size_t align = ( vertexStride - ( verticesNum % vertexStride ) ) % vertexStride;
					verticesNum += align + renderObject->verticesNum;
					//bool batch = s_checkForBatch( batchedRO, renderObject );
					//if( batch == true )
					//{
					//	batchedRO->dipIndiciesNum += renderObject->dipIndiciesNum;
					//	batchedRO->dipVerticesNum += renderObject->verticesNum;
					//	renderObject->dipVerticesNum = 0;
					//	renderObject->dipIndiciesNum = 0;

					//	renderObject->baseVertexIndex = 0;
					//}
					//else
					//{
					//batchedRO = renderObject;
					renderObject->dipVerticesNum = renderObject->verticesNum;
					renderObject->baseVertexIndex = 0;
					//}
				}break;
			}
		}

		return (verticesNum - _startVertexPos);
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderEngine::insertRenderObjects_( RenderPass * _pass, Vertex2D * _vertexBuffer, size_t _offset )
	{
		TVectorRenderObject & renderObjects = _pass->renderObjects;

		float texelOffsetX = m_interface->getTexelOffsetX();
		float texelOffsetY = m_interface->getTexelOffsetY();

		const mt::mat3f & pass_wm = _pass->wm;

		for( TVectorRenderObject::iterator
			it = renderObjects.begin(), 
			it_end = renderObjects.end();
		it != it_end;
		++it )
		{
			RenderObject* ro = &(*it);

			ELogicPrimitiveType type = ro->logicPrimitiveType;

			switch(type)
			{
			case LPT_MESH:
				{
					if( m_vbPos + ro->verticesNum > m_maxPrimitiveVertices2D )
					{
						MENGE_LOG_ERROR("Warning: vertex buffer overflow");
						ro->verticesNum = m_maxPrimitiveVertices2D - m_vbPos;
						//return;
					}

					ro->startIndex = 0;
					ro->minIndex = 0;
				}break;
			default:
				{
					std::size_t indexStart = m_primitiveIndexStart[type];
					std::size_t indexStride = m_primitiveIndexStride[type];
					std::size_t vertexStride = m_primitiveVertexStride[type];

					size_t align = ( vertexStride - ( m_vbPos % vertexStride ) ) % vertexStride;
					_offset += align;
					m_vbPos += align;
					if( m_vbPos + ro->verticesNum > m_maxPrimitiveVertices2D )
					{
						MENGE_LOG_ERROR("Warning: vertex buffer overflow");
						ro->verticesNum = m_maxPrimitiveVertices2D - m_vbPos;
						//return;
					}

					ro->startIndex  = indexStart + m_vbPos / vertexStride * indexStride;
					ro->minIndex = (ro->startIndex - indexStart) / indexStride * vertexStride;

					assert( ro->startIndex + ro->dipIndiciesNum <= m_maxIndexCount );
				}break;
			}

			//Vertex2D * vertexBuffer = _vertexBuffer + _offset * sizeof(Vertex2D)
			Vertex2D * offsetVertexBuffer = _vertexBuffer + _offset;
			std::copy( ro->vertexData, ro->vertexData + ro->verticesNum, offsetVertexBuffer );

			for( Vertex2D
				* it = offsetVertexBuffer,
				* it_end = offsetVertexBuffer + ro->verticesNum;
			it != it_end;
			++it )
			{
				mt::vec2f * v = (mt::vec2f *)&it->pos;

				mt::vec2f nv;
				mt::mul_v2_m3( nv, *v, pass_wm );

				nv.x *= m_renderScale.x;
				nv.y *= m_renderScale.y;

				nv.x += m_renderOffset.x;
				nv.y += m_renderOffset.y;

				nv.x += texelOffsetX;
				nv.y += texelOffsetY;

				it->pos[0] = nv.x;
				it->pos[1] = nv.y;
			}

			m_vbPos += ro->verticesNum;
			_offset += ro->verticesNum;

			//m_logicPrimitiveCount[type] += 1;
		}
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
			MENGE_LOG_WARNING("Warning: vertex buffer overflow");
			//vertexDataSize = m_maxPrimitiveVertices2D;
			setRenderSystemDefaults_( m_primitiveCount[LPT_QUAD] * 2 );
			recreate2DBuffers_( m_maxIndexCount );
			restoreRenderSystemStates_();
			prepare2D_();
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
			m_interface->setVertexDeclaration( m_currentVertexDeclaration );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::prepare3D_()
	{		
		if( m_currentVertexDeclaration != Vertex3D_declaration )
		{
			m_currentVertexDeclaration = Vertex3D_declaration;
			m_interface->setVertexDeclaration( m_currentVertexDeclaration );
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
			i + m_primitiveIndexStride[LPT_QUAD] < m_primitiveIndexStart[LPT_TRIANGLE];
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
		vertexCount = 0;
		for( size_t i = m_primitiveIndexStart[LPT_TRIANGLE];
			i + m_primitiveIndexStride[LPT_TRIANGLE] < m_primitiveIndexStart[LPT_LINE];
			i += 3, vertexCount += 3 )
		{
			ibuffer[i+0] = 0 + vertexCount;
			ibuffer[i+1] = 1 + vertexCount;
			ibuffer[i+2] = 2 + vertexCount;
		}
		maxVertices = std::min( maxVertices, vertexCount );
		// LINES
		vertexCount = 0;
		for( size_t i = m_primitiveIndexStart[LPT_LINE];
			i + m_primitiveIndexStride[LPT_LINE] < m_primitiveIndexStart[LPT_RECTANGLE];
			i += 1, vertexCount += 1 )
		{
			ibuffer[i+0] = 0 + vertexCount;
		}
		maxVertices = std::min( maxVertices, vertexCount );
		// RECTANGLES
		vertexCount = 0;
		for( size_t i = m_primitiveIndexStart[LPT_RECTANGLE];
			i + m_primitiveIndexStride[LPT_RECTANGLE] < m_maxIndexCount;
			i += 5, vertexCount += 4 )
		{
			ibuffer[i+0] = 0 + vertexCount;
			ibuffer[i+1] = 1 + vertexCount;
			ibuffer[i+2] = 2 + vertexCount;
			ibuffer[i+3] = 3 + vertexCount;
			ibuffer[i+4] = 0 + vertexCount;
		}

		maxVertices = std::min( maxVertices, vertexCount );

		if( m_interface->unlockIndexBuffer( m_ibHandle2D ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to unlock index buffer" );
		}
		return maxVertices;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::recreate2DBuffers_( std::size_t _maxIndexCount )
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
		m_primitiveIndexStride[LPT_TRIANGLE] = 3;
		m_primitiveVertexStride[LPT_TRIANGLE] = 3;
		m_primitiveIndexStride[LPT_LINE] = 1;
		m_primitiveVertexStride[LPT_LINE] = 1;
		m_primitiveIndexStride[LPT_RECTANGLE] = 5;
		m_primitiveVertexStride[LPT_RECTANGLE] = 4;

		for( std::size_t i = 1; i != LPT_PRIMITIVE_COUNT; ++i )
		{
			m_primitiveCount[i] = m_primitiveCount[LPT_QUAD] * m_primitiveVertexStride[LPT_QUAD] / m_primitiveVertexStride[i];
		}

		m_primitiveIndexStart[LPT_QUAD] = 0;
		for( std::size_t i = 0; i != LPT_PRIMITIVE_COUNT - 1; ++i )
		{
			m_primitiveIndexStart[i+1] = m_primitiveIndexStart[i] + m_primitiveCount[i] * m_primitiveIndexStride[i];
		}

		for( std::size_t i = 0; i != LPT_PRIMITIVE_COUNT; ++i )
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

		for( int i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			ETextureFilter tFilter = m_textureFiltering ? TF_LINEAR : TF_NONE;

			m_interface->setTextureStageFilter( i, TFT_MIPMAP, TF_NONE );
			m_interface->setTextureStageFilter( i, TFT_MAGNIFICATION, tFilter );
			m_interface->setTextureStageFilter( i, TFT_MINIFICATION, tFilter );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::isTextureFilteringEnabled() const
	{
		return m_textureFiltering;
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
}
