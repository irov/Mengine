#	include "RenderEngine.h"

#	include "Logger/Logger.h"

#	include "Interface/CodecInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ImageCodecInterface.h"
#   include "Interface/FileSystemInterface.h"

#	include "RenderSubTexture.h"

#	include "Megatextures.h"

#	include "Core/PixelFormat.h"

#	include <algorithm>
#	include <map>
#	include <ctime>

#   include "Interface/WatchdogInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderService, Menge::RenderServiceInterface, Menge::RenderEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static Menge::uint32 s_firstPO2From( Menge::uint32 n )
    {
        --n;            
        n |= n >> 16;
        n |= n >> 8;
        n |= n >> 4;
        n |= n >> 2;
        n |= n >> 1;
        ++n;
        return n;
    }
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine()
		: m_serviceProvider(nullptr)
        , m_maxQuadCount(0)
		, m_windowCreated(false)
		, m_vsync(false)
		, m_layer3D(false)
		, m_vbHandle2D(0)
		, m_ibHandle2D(0)
		, m_currentVBHandle(0)
		, m_currentIBHandle(0)
		, m_currentBaseVertexIndex(0)
		, m_currentTextureStages(0)
		, m_currentRenderCamera(nullptr)
		, m_nullTexture(nullptr)
		, m_currentVertexDeclaration(0)
		, m_maxPrimitiveVertices2D(0)
		, m_maxIndexCount(0)
        , m_idEnumerator(0)
        , m_vbPos(0)
        , m_currentRenderPass(nullptr)
		, m_depthBufferWriteEnable(false)
		, m_alphaBlendEnable(false)
		, m_alphaTestEnable(false)
		, m_textureFiltering(true)		
		//, m_renderScale(0.f, 0.f)
		//, m_renderOffset(0.f, 0.f)
		//, m_megatextures(NULL)
        , m_debugMode(false)
        , m_currentMaterial(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::~RenderEngine()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;

        //m_renderSystem = RENDER_SYSTEM( m_serviceProvider );

        RENDER_SYSTEM(m_serviceProvider)
            ->setRenderListener( this );
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * RenderEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderEngine::initialize( size_t _maxQuadCount )
    {
        m_maxQuadCount = _maxQuadCount;

		this->setRenderSystemDefaults_( m_maxQuadCount );

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
		
		//m_megatextures = new Megatextures(2048.f, 2048.f, PF_A8R8G8B8);

        m_defaultRenderTarget = Helper::stringizeString(m_serviceProvider, "Window");

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::finalize()
	{
		m_nullTexture = nullptr;
        m_renderObjects.clear();
        m_renderPasses.clear();
    	//m_textures.clear();

		for( TMapMaterialGroup::iterator
			it = m_mapMaterialGroup.begin(),
			it_end = m_mapMaterialGroup.end();
		it != it_end;
		++it )
		{
			delete it->second;
		}

		m_mapMaterialGroup.clear();

        RENDER_SYSTEM(m_serviceProvider)->releaseVertexBuffer( m_vbHandle2D );
        RENDER_SYSTEM(m_serviceProvider)->releaseIndexBuffer( m_vbHandle2D );

        for( TVectorVertexBuffer::iterator
            it = m_vertexBuffer.begin(),
            it_end = m_vertexBuffer.end();
        it != it_end;
        ++it )
        {
            VBHandle handle = *it;

            RENDER_SYSTEM(m_serviceProvider)
                ->releaseVertexBuffer( handle );
        }

        m_vertexBuffer.clear();

        for( TVectorIndexBuffer::iterator
            it = m_indexBuffer.begin(),
            it_end = m_indexBuffer.end();
        it != it_end;
        ++it )
        {
            IBHandle handle = *it;

            RENDER_SYSTEM(m_serviceProvider)
                ->releaseIndexBuffer( handle );
        }

        m_indexBuffer.clear();

		//delete m_megatextures;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, int _bits, bool _fullscreen,
		WindowHandle _winHandle, int _FSAAType, int _FSAAQuality )
	{
		m_windowResolution = _resolution;
		m_contentResolution = _contentResolution;
		m_renderViewport = _renderViewport;

		m_fullscreen = _fullscreen;

        LOGGER_INFO(m_serviceProvider)("RenderEngine::createRenderWindow:\nwindow resolution [%d, %d]\ncontent resolution [%d, %d]\nrender viewport [%f %f %f %f]\nfullscreen %d"
            , m_windowResolution.getWidth()
            , m_windowResolution.getHeight()
            , m_contentResolution.getWidth()
            , m_contentResolution.getHeight()
            , m_renderViewport.begin.x
            , m_renderViewport.begin.y
            , m_renderViewport.end.x
            , m_renderViewport.end.y
            , m_fullscreen
            );
		
		size_t width = m_windowResolution.getWidth();
		size_t height = m_windowResolution.getHeight();

		m_windowCreated = RENDER_SYSTEM(m_serviceProvider)
            ->createRenderWindow( width, height, _bits, m_fullscreen, _winHandle, m_vsync, _FSAAType, _FSAAQuality );

		if( m_windowCreated == false )
		{
			return false;
		}

		m_debugInfo.dips = 0;
		m_debugInfo.textureMemory = 0;
		m_debugInfo.textureCount = 0;
		m_debugInfo.frameCount = 0;
        m_debugInfo.fillrate = 0.f;
        m_debugInfo.object = 0;
        m_debugInfo.triangle = 0;
		//m_debugInfo.megatextures = 0;

		if( this->recreate2DBuffers_( m_maxIndexCount ) == false )
		{
			return false;
		}

		this->restoreRenderSystemStates_();
		this->prepare2D_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::createWhitePixelTexture_()
	{
		size_t null_width = 2;
		size_t null_height = 2;
        size_t null_channels = 3;

		m_nullTexture = this->createTexture( null_width, null_height, null_channels, PF_UNKNOWN, null_width, null_height );

		if( m_nullTexture == NULL )
		{
			return;
		}
		
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
		
		this->cacheFileTexture( Helper::stringizeString(m_serviceProvider, "__null__"), m_nullTexture );
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen )
	{
		m_windowResolution = _resolution;
		m_contentResolution = _contentResolution;
		m_renderViewport = _renderViewport;
		
		m_fullscreen = _fullscreen;

        LOGGER_INFO(m_serviceProvider)("RenderEngine::changeWindowMode:\nwindow resolution [%d, %d]\ncontent resolution [%d, %d]\nrender viewport [%f %f %f %f]\nfullscreen %d"
            , m_windowResolution.getWidth()
            , m_windowResolution.getHeight()
            , m_contentResolution.getWidth()
            , m_contentResolution.getHeight()
            , m_renderViewport.begin.x
            , m_renderViewport.begin.y
            , m_renderViewport.end.x
            , m_renderViewport.end.y
            , m_fullscreen
            );
	
		if( m_windowCreated == false )
		{
			return;
		}

		RENDER_SYSTEM(m_serviceProvider)
            ->changeWindowMode( m_windowResolution, m_fullscreen );

		this->restoreRenderSystemStates_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::screenshot( const RenderTextureInterfacePtr & _texture, const mt::vec4f & _rect )
	{
		RenderImageInterfacePtr image = _texture->getImage();

		RENDER_SYSTEM(m_serviceProvider)
            ->screenshot( image, _rect.buff() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createMaterialGroup( const ConstString & _name, const RenderMaterial & _material )
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
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::getMaterialGroup: not exsist RenderMaterial '%s'"
				, _name.c_str()
				);

			return nullptr;
		}

        RenderMaterialGroup * materialGroup = it_found->second;

		return materialGroup;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::removeMaterialGroup( const ConstString & _name )
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
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr RenderEngine::createTexture( size_t _imageWidth, size_t _imageHeight, size_t _imageChannels, PixelFormat _imageFormat, size_t _textureWidth, size_t _textureHeight )
	{
        std::size_t pow2_width = _imageWidth;
        std::size_t pow2_height = _imageHeight;

        if( ( _imageWidth & ( _imageWidth - 1 ) ) != 0 || ( _imageHeight & ( _imageHeight - 1 ) ) != 0 )
        {
            pow2_width = s_firstPO2From( _imageWidth );
            pow2_height = s_firstPO2From( _imageHeight );
        }

        RenderImageInterfacePtr image = RENDER_SYSTEM(m_serviceProvider)
            ->createImage( pow2_width, pow2_height, _imageChannels, _imageFormat );

        if( image == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "RenderEngine::createTexture_ couldn't create image %dx%d"
                , pow2_width
                , pow2_height
                );

            return nullptr;
        }

		size_t id = ++m_idEnumerator;

		RenderTexture * texture = m_factoryRenderTexture.createObjectT();
        texture->initialize( image, _textureWidth, _textureHeight, _imageChannels, id, this );

		size_t memroy_size = texture->getMemoryUse();

        size_t HWWidth = texture->getHWWidth();
        size_t HWHeight = texture->getHWHeight();
        PixelFormat HWPixelFormat = texture->getHWPixelFormat();

		m_debugInfo.textureMemory += memroy_size;
		++m_debugInfo.textureCount;

        LOGGER_INFO(m_serviceProvider)( "RenderEngine::createTexture creating texture %dx%d %d memory %d:%d"
            , HWWidth
            , HWHeight
            , HWPixelFormat
            , memroy_size
            , m_debugInfo.textureMemory
            );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr RenderEngine::createDynamicTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
	{
        std::size_t pow2_width = _width;
        std::size_t pow2_height = _height;

        if( ( _width & ( _width - 1 ) ) != 0 || ( _height & ( _height - 1 ) ) != 0 )
        {
            pow2_width = s_firstPO2From( _width );
            pow2_height = s_firstPO2From( _height );
        }

        RenderImageInterfacePtr image = RENDER_SYSTEM(m_serviceProvider)
            ->createDynamicImage( pow2_width, pow2_height, _channels, _format );

        if( image == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "RenderEngine::createDynamicTexture couldn't create image %dx%d channels %d"
                , pow2_width
                , pow2_height
                , _channels
                );

            return nullptr;
        }

		size_t id = ++m_idEnumerator;

        RenderTexture * texture = m_factoryRenderTexture.createObjectT();
        texture->initialize( image, _width, _height, _channels, id, this );

		size_t memroy_size = texture->getMemoryUse();

		m_debugInfo.textureMemory += memroy_size;
		++m_debugInfo.textureCount;

        LOGGER_INFO(m_serviceProvider)( "RenderEngine::createDynamicTexture creating dynamic texture %dx%d channels %d use memory %d:%d"
            , pow2_width
            , pow2_height
            , _channels 
            , memroy_size
            , m_debugInfo.textureMemory
            );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::saveImage( const RenderTextureInterfacePtr & _texture, const ConstString & _fileSystemName, const FilePath & _filename )
	{
		OutputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( _fileSystemName, _filename );

		if( stream == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)( "RenderEngine::saveImage : can't create file '%s' '%s'"
				, _fileSystemName.c_str()
				, _filename.c_str() 
				);

			return false;
		}

		ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE(m_serviceProvider)
            ->createEncoderT<ImageEncoderInterfacePtr>( Helper::stringizeString(m_serviceProvider, "pngImage"), stream );

		if( imageEncoder == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "RenderEngine::saveImage : can't create encoder for filename '%s'"
				, _filename.c_str() 
				);

			return false;
		}

		ImageCodecDataInfo dataInfo;
		//dataInfo.format = _image->getHWPixelFormat();
        dataInfo.width = _texture->getWidth();
		dataInfo.height = _texture->getHeight();		
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
		
		unsigned char * buffer = _texture->lock( &pitch, rect, true );
        
		ImageCodecOptions options;		

		options.pitch = pitch;
        options.channels = 4;
        //options.flags |= DF_CUSTOM_PITCH;

		imageEncoder->setOptions( &options );

		unsigned int bytesWritten = imageEncoder->encode( buffer, &dataInfo );

		_texture->unlock();

		if( bytesWritten == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "RenderEngine::saveImage : Error while encoding image data"
                );

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::cacheFileTexture( const FilePath& _filename, const RenderTextureInterfacePtr & _texture )
	{
		_texture->setFileName( _filename );

        RenderTextureInterface * texture_ptr = _texture.get();
        m_textures.insert( std::make_pair(_filename, texture_ptr) );

        LOGGER_INFO(m_serviceProvider)( "RenderEngine::cacheFileTexture cache texture %s %d:%d"
            , _filename.c_str()
            , _texture->getHWWidth()
            , _texture->getHWHeight()
            );
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr RenderEngine::getTexture( const FilePath& _filename )
	{
		TMapTextures::iterator it_find = m_textures.find( _filename );

		if( it_find == m_textures.end() )
		{
            return nullptr;
        }
         
        RenderTextureInterface * texture = it_find->second;
        		
		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr RenderEngine::loadTexture( const ConstString& _pakName, const FilePath & _filename, const ConstString& _codec, size_t _width, size_t _height )
	{
		TMapTextures::iterator it_find = m_textures.find( _filename );
	
		if( it_find != m_textures.end() )
		{
            RenderTextureInterface * texture = it_find->second;

			return texture;
		}
		
		InputStreamInterfacePtr stream = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( _pakName, _filename );
		
		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "RenderEngine::loadTexture: Image file '%s' was not found"
				, _filename.c_str() 
				);

			return nullptr;
		}	

		ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(m_serviceProvider)
            ->createDecoderT<ImageDecoderInterfacePtr>( _codec, stream );

		if( imageDecoder == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "RenderEngine::loadTexture: Image decoder for file '%s' was not found"
				, _filename.c_str() 
				);

			return nullptr;
		}

		const ImageCodecDataInfo* dataInfo = imageDecoder->getCodecDataInfo();
        	
        BEGIN_WATCHDOG(m_serviceProvider, "texture create");

        size_t image_width = _width > dataInfo->width? _width: dataInfo->width;
        size_t image_height = _height > dataInfo->height? _height: dataInfo->height;
        size_t image_channels = dataInfo->channels;

        size_t texture_width = dataInfo->width;
        size_t texture_height = dataInfo->height;

		RenderTextureInterfacePtr texture = this->createTexture( image_width, image_height, image_channels, dataInfo->format, texture_width, texture_height );

        END_WATCHDOG(m_serviceProvider, "texture create")("texture create %s"
            , _filename.c_str()
            );
		
		if( texture == nullptr )
		{
            LOGGER_ERROR(m_serviceProvider)( "RenderEngine::loadTexture: invalid create texture %d:%d channels %d format %d for file '%s'"
                , image_width
                , image_height
                , image_channels
                , dataInfo->format
                , _filename.c_str() 
                );

			return nullptr;
		}

		const Rect & rect = texture->getHWRect();
		if( this->loadTextureRectImageData( texture, rect, imageDecoder ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "RenderEngine::loadTexture: decode texture for file '%s:%s' error"
                , _pakName.c_str()
                , _filename.c_str() 
                );

            return nullptr;
        }		

        this->cacheFileTexture( _filename, texture );
		
		return texture;
	}
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::onRenderTextureRelease( const RenderTextureInterface * _texture )
    {
        const FilePath & filename = _texture->getFileName();

        m_textures.erase( filename );

        size_t memroy_size = _texture->getMemoryUse();

        size_t HWWidth = _texture->getHWWidth();
        size_t HWHeight = _texture->getHWHeight();
        PixelFormat HWPixelFormat = _texture->getHWPixelFormat();

        m_debugInfo.textureMemory -= memroy_size;
        --m_debugInfo.textureCount;

        LOGGER_INFO(m_serviceProvider)( "RenderEngine::destroyTexture_ destroy texture %dx%d %d memory %d:%d"
            , HWWidth
            , HWHeight
            , HWPixelFormat
            , memroy_size
            , m_debugInfo.textureMemory
            );
    }
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::loadTextureRectImageData( const RenderTextureInterfacePtr & _texture, const Rect & _rect, const ImageDecoderInterfacePtr & _imageDecoder )
	{
		int pitch = 0;
		unsigned char * textureBuffer = _texture->lock( &pitch, _rect, false );

		if( textureBuffer == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::loadTextureImageData Invalid lock");

			return false;
		}

        ImageCodecOptions options;

        options.channels = _texture->getHWChannels();
        options.pitch = pitch;
		
		//options.flags |= DF_CUSTOM_PITCH;

		_imageDecoder->setOptions( &options );
		
		//bool result = this->loadBufferImageData( textureBuffer, pitch, hwPixelFormat, _imageDecoder );

        const ImageCodecDataInfo * data = _imageDecoder->getCodecDataInfo();

        BEGIN_WATCHDOG(m_serviceProvider, "texture decode");

        unsigned int bufferSize = pitch * data->height;
        if( _imageDecoder->decode( textureBuffer, bufferSize ) == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::loadTextureImageData Invalid decode");

            _texture->unlock();

            return false;
        }

        END_WATCHDOG(m_serviceProvider, "texture decode")("texture decode %s"
            , _texture->getFileName().c_str()
            );

        //this->sweezleAlpha( _texture, textureBuffer, pitch );
        this->imageQuality( _texture, textureBuffer, pitch );

        BEGIN_WATCHDOG(m_serviceProvider, "texture unlock");

		_texture->unlock();

        END_WATCHDOG(m_serviceProvider, "texture unlock")("texture unlock %.4f %s"
            , _texture->getFileName().c_str()
            );


		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	//void RenderEngine::sweezleAlpha( RenderTextureInterface * _texture, unsigned char* _textureBuffer, size_t _texturePitch )
	//{   
 //       size_t channels = _texture->getChannels();
 //       size_t hwChannels = _texture->getHWChannels();

	//	// need to sweezle alpha
	//	if( channels == PF_A8
	//		&& hwChannels == PF_A8R8G8B8 )
	//	{            
 //           size_t width = _texture->getWidth();
 //           size_t height = _texture->getHeight();

	//		for( size_t h = height - 1; h != -1; --h )
	//		{
	//			size_t hp = h * _texturePitch;

	//			for( size_t w = width - 1; w != -1; --w )
	//			{
	//				_textureBuffer[hp + w*4 + 3] = _textureBuffer[hp + w];
	//			}
	//		}
	//	}
	//}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::imageQuality( const RenderTextureInterfacePtr & _texture, unsigned char * _textureBuffer, size_t _texturePitch )
	{
        size_t width = _texture->getWidth();
        size_t height = _texture->getHeight();

        size_t hwWidth = _texture->getHWWidth();
        size_t hwHeight = _texture->getHWHeight();


		// copy pixels on the edge for better image quality
        if( hwWidth > width )
        {
            LOGGER_INFO(m_serviceProvider)("imageQuality_ width %d - %d"
                , width
                , hwWidth
                );

            unsigned char* image_data = _textureBuffer;
            unsigned int pixel_size = _texturePitch / hwWidth;

            for( size_t j = 0; j != height; ++j )
            {
                std::copy( image_data + (width - 1) * pixel_size,
                    image_data + width * pixel_size,
                    image_data + width * pixel_size );


                image_data += _texturePitch;
            }
        }

        if( hwHeight > height )
        {
            LOGGER_INFO(m_serviceProvider)("imageQuality_ height %d - %d"
                , height
                , hwHeight
                );

            unsigned char* image_data = _textureBuffer;

            std::copy( image_data + (height - 1) * _texturePitch,
                image_data + height * _texturePitch,
                image_data + height * _texturePitch );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::onRenderSystemDeviceRestored()
	{
        LOGGER_WARNING(m_serviceProvider)("RenderEngine::onDeviceRestored"
            );

		if( this->refillIndexBuffer2D_( m_maxPrimitiveVertices2D ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderEngine::onRenderSystemDeviceRestored invalid refill index buffer"
                );

            return false;
        }

		this->restoreRenderSystemStates_();

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onWindowClose()
	{
		if( m_windowCreated )
		{
			RENDER_SYSTEM(m_serviceProvider)
                ->onWindowClose();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::beginScene()
	{		
		m_renderPasses.clear();
		m_renderObjects.clear();

        m_debugInfo.fillrate = 0.f;
        m_debugInfo.object = 0;
        m_debugInfo.triangle = 0;

		m_currentRenderCamera = NULL;

		m_currentRenderTarget = m_defaultRenderTarget;
		m_renderTargetResolution = m_windowResolution;
		
		if( RENDER_SYSTEM(m_serviceProvider)->beginScene() == false )
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

		RENDER_SYSTEM(m_serviceProvider)
            ->endScene();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::swapBuffers()
	{
		RENDER_SYSTEM(m_serviceProvider)
            ->swapBuffers();

		m_debugInfo.frameCount += 1;		
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderTarget( const ConstString& _target, bool _clear )
	{
        (void)_clear;

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
	void RenderEngine::renderObject_( const RenderObject* _renderObject )
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
			RenderTextureInterfacePtr texture = _renderObject->textures[stageId];

			if( texture == nullptr )
			{
				texture = m_nullTexture;
			}
		
            size_t texture_id = texture->getId();
            
			if( texture_id != m_currentTexturesID[stageId] || m_currentTexturesID[stageId] != 0 )
			{
				m_currentTexturesID[stageId] = texture_id;
				RenderImageInterfacePtr image = texture->getImage();

				RENDER_SYSTEM(m_serviceProvider)
                    ->setTexture( stageId, image );
			}
        }

        if( m_currentMaterial != material )
        {
            for( size_t stageId = 0; stageId != m_currentTextureStages; ++stageId )
            {
                RenderTextureStage & current_stage = m_currentTextureStage[stageId];
                const RenderTextureStage & stage = material->textureStage[stageId];

                if( current_stage.filter != stage.filter )
                {
                    current_stage.filter = stage.filter;

                    RENDER_SYSTEM(m_serviceProvider)
                        ->setTextureStageFilter( stageId, TFT_MAGNIFICATION, current_stage.filter );

                    RENDER_SYSTEM(m_serviceProvider)
                        ->setTextureStageFilter( stageId, TFT_MINIFICATION, current_stage.filter );
                }

                if( current_stage.addressU != stage.addressU
                    || current_stage.addressV != stage.addressV )
                {
                    current_stage.addressU = stage.addressU;
                    current_stage.addressV = stage.addressV;

                    RENDER_SYSTEM(m_serviceProvider)->setTextureAddressing( stageId
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

                    RENDER_SYSTEM(m_serviceProvider)->setTextureStageColorOp( stageId
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

                    RENDER_SYSTEM(m_serviceProvider)->setTextureStageAlphaOp( stageId
                        , current_stage.alphaOp
                        , current_stage.alphaArg1
                        , current_stage.alphaArg2 );
                }

                if( current_stage.texCoordIndex != stage.texCoordIndex )
                {
                    current_stage.texCoordIndex = stage.texCoordIndex;

                    RENDER_SYSTEM(m_serviceProvider)->setTextureStageTexCoordIndex( stageId
                        , current_stage.texCoordIndex
                        );
                }
            }

			if( m_depthBufferWriteEnable != material->depthBufferWriteEnable )
			{
				m_depthBufferWriteEnable = material->depthBufferWriteEnable;

				RENDER_SYSTEM(m_serviceProvider)
                    ->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
			}

			if( m_alphaTestEnable != material->alphaTestEnable )
			{
				m_alphaTestEnable = material->alphaTestEnable;

				RENDER_SYSTEM(m_serviceProvider)
                    ->setAlphaTestEnable( m_alphaTestEnable );
			}

			if( m_alphaBlendEnable != material->alphaBlendEnable )
			{
				m_alphaBlendEnable = material->alphaBlendEnable;

				RENDER_SYSTEM(m_serviceProvider)
                    ->setAlphaBlendEnable( m_alphaBlendEnable );
			}

			if( m_currentBlendSrc != material->blendSrc )
			{
				m_currentBlendSrc = material->blendSrc;

				RENDER_SYSTEM(m_serviceProvider)
                    ->setSrcBlendFactor( m_currentBlendSrc );
			}

			if( m_currentBlendDst != material->blendDst )
			{
				m_currentBlendDst = material->blendDst;

				RENDER_SYSTEM(m_serviceProvider)
                    ->setDstBlendFactor( m_currentBlendDst );
			}

            m_currentMaterial = material;
		}

		if( m_currentIBHandle != _renderObject->ibHandle || 
			m_currentBaseVertexIndex != _renderObject->baseVertexIndex )
		{
			m_currentIBHandle = _renderObject->ibHandle;
			m_currentBaseVertexIndex = _renderObject->baseVertexIndex;

			RENDER_SYSTEM(m_serviceProvider)
                ->setIndexBuffer( m_currentIBHandle, m_currentBaseVertexIndex );
		}

		RENDER_SYSTEM(m_serviceProvider)->drawIndexedPrimitive( 
			_renderObject->primitiveType, 
			_renderObject->baseVertexIndex, 
			_renderObject->minIndex,
			_renderObject->dipVerticesNum, 
			_renderObject->startIndex, 
			_renderObject->dipIndiciesNum 
			);

		++m_debugInfo.dips;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::disableTextureStage_( size_t _stage )
	{
		RenderTextureStage & stage = m_currentTextureStage[_stage];

        stage = RenderTextureStage();

		m_currentTexturesID[_stage] = 0;

		RENDER_SYSTEM(m_serviceProvider)
            ->setTexture( _stage, nullptr );

        RENDER_SYSTEM(m_serviceProvider)->setTextureAddressing( _stage
            , stage.addressU
            , stage.addressV );

		RENDER_SYSTEM(m_serviceProvider)->setTextureStageColorOp( _stage
			, stage.colorOp
			, stage.colorArg1
			, stage.colorArg2
			);

        RENDER_SYSTEM(m_serviceProvider)->setTextureStageAlphaOp( _stage
            , stage.alphaOp
            , stage.alphaArg1
            , stage.alphaArg2
            );

        RENDER_SYSTEM(m_serviceProvider)->setTextureStageTexCoordIndex( _stage
            , 0 
            );
        
		RENDER_SYSTEM(m_serviceProvider)
            ->setTextureMatrix( _stage, nullptr );
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

		RENDER_SYSTEM(m_serviceProvider)->setVertexBuffer( m_currentVBHandle );
		RENDER_SYSTEM(m_serviceProvider)->setIndexBuffer( m_currentIBHandle, m_currentBaseVertexIndex );
		RENDER_SYSTEM(m_serviceProvider)->setVertexDeclaration( sizeof(Vertex2D), Vertex2D_declaration );
		RENDER_SYSTEM(m_serviceProvider)->setProjectionMatrix( projTransform );
		RENDER_SYSTEM(m_serviceProvider)->setModelViewMatrix( viewTransform );
		RENDER_SYSTEM(m_serviceProvider)->setWorldMatrix( worldTransform );
		RENDER_SYSTEM(m_serviceProvider)->setCullMode( CM_CULL_NONE );
		RENDER_SYSTEM(m_serviceProvider)->setFillMode( FM_SOLID );
		RENDER_SYSTEM(m_serviceProvider)->setDepthBufferTestEnable( false );
		RENDER_SYSTEM(m_serviceProvider)->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
		RENDER_SYSTEM(m_serviceProvider)->setDepthBufferCmpFunc( CMPF_LESS_EQUAL );
		RENDER_SYSTEM(m_serviceProvider)->setAlphaTestEnable( m_alphaTestEnable );
		RENDER_SYSTEM(m_serviceProvider)->setAlphaBlendEnable( m_alphaBlendEnable );
		RENDER_SYSTEM(m_serviceProvider)->setAlphaCmpFunc( CMPF_GREATER_EQUAL, 0x01 );
		RENDER_SYSTEM(m_serviceProvider)->setLightingEnable( false );
		
        LOGGER_WARNING(m_serviceProvider)("RenderEngine::restoreRenderSystemStates_ texture stages %d"
            , MENGE_MAX_TEXTURE_STAGES
            );

		for( int i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			RenderTextureStage & stage = m_currentTextureStage[i];

			RENDER_SYSTEM(m_serviceProvider)->setTextureAddressing( i, stage.addressU, stage.addressV );

			RENDER_SYSTEM(m_serviceProvider)->setTextureStageColorOp( i
				, stage.colorOp
				, stage.colorArg1
				, stage.colorArg2 
				);

			RENDER_SYSTEM(m_serviceProvider)->setTextureStageAlphaOp( i
				, stage.alphaOp
				, stage.alphaArg1
				, stage.alphaArg2 );

            RENDER_SYSTEM(m_serviceProvider)->setTextureStageTexCoordIndex( i
                , stage.texCoordIndex
                );

			RENDER_SYSTEM(m_serviceProvider)->setTextureStageFilter( i, TFT_MIPMAP, stage.filter );
			RENDER_SYSTEM(m_serviceProvider)->setTextureStageFilter( i, TFT_MAGNIFICATION, stage.filter );
			RENDER_SYSTEM(m_serviceProvider)->setTextureStageFilter( i, TFT_MINIFICATION, stage.filter );
			
			// skip texture matrix
			RENDER_SYSTEM(m_serviceProvider)->setTextureMatrix( i, NULL );
		}

		RENDER_SYSTEM(m_serviceProvider)->setSrcBlendFactor( m_currentBlendSrc );
		RENDER_SYSTEM(m_serviceProvider)->setDstBlendFactor( m_currentBlendDst );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeProjectionOrthogonal( mt::mat4f& _projectionMatrix, const Viewport & _viewport, float zn, float zf )
	{
		RENDER_SYSTEM(m_serviceProvider)->makeProjectionOrthogonal( _projectionMatrix, _viewport, zn, zf );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf )
	{
		RENDER_SYSTEM(m_serviceProvider)->makeProjectionPerspective( _projectionMatrix, _fov, _aspect, zn, zf );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float zn, float zf )
	{
		RENDER_SYSTEM(m_serviceProvider)->makeProjectionFrustum( _projectionMatrix, _viewport, zn, zf );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeViewMatrixFromViewport( mt::mat4f& _viewMatrix, const Viewport & _viewport )
	{
        RENDER_SYSTEM(m_serviceProvider)->makeViewMatrixFromViewport( _viewMatrix, _viewport );
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderDebugInfo& RenderEngine::getDebugInfo() const
	{
		return m_debugInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::hasTexture( const FilePath & _filename, RenderTextureInterfacePtr * _texture ) const
	{
		TMapTextures::const_iterator it_find = m_textures.find( _filename );

		if( it_find == m_textures.end() )
		{
			return false;
		}

        if( _texture != nullptr )
        {
            *_texture = it_find->second;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::resetFrameCount()
	{
		m_debugInfo.frameCount = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderPasses_()
	{
		if( m_renderPasses.empty() == true )	// nothing to render
		{
			return;
		}

		for( TVectorRenderPass::const_iterator 
			rit = m_renderPasses.begin(), 
			rit_end = m_renderPasses.end();
		rit != rit_end;
		++rit )
		{
			const RenderPass & renderPass = *rit;

			this->renderPass_( renderPass );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderPass_( const RenderPass & _renderPass )
	{
		const RenderCameraInterface * camera = _renderPass.camera;

		const ConstString& renderTarget = camera->getRenderTarget();

		if( renderTarget != m_currentRenderTarget && renderTarget.empty() == false )
		{
			m_currentRenderTarget = renderTarget;
			RENDER_SYSTEM(m_serviceProvider)->setRenderTarget( NULL, true );

			m_renderTargetResolution = m_windowResolution;
		}
		
		const Viewport & viewport = camera->getViewport();

		float windowWidth = (float)m_windowResolution.getWidth();
		float windowHeight = (float)m_windowResolution.getHeight();

		Viewport renderViewport;

		renderViewport.begin.x = viewport.begin.x / windowWidth;
		renderViewport.begin.y = viewport.begin.y / windowHeight;
		renderViewport.end.x = viewport.end.x / windowWidth;
		renderViewport.end.y = viewport.end.y / windowHeight;

		float renderWidth = m_renderViewport.getWidth();
		float renderHeight = m_renderViewport.getHeight();

		renderViewport.begin.x *= renderWidth;
		renderViewport.begin.y *= renderHeight;
		renderViewport.end.x *= renderWidth;
		renderViewport.end.y *= renderHeight;

		renderViewport.begin += m_renderViewport.begin;
		renderViewport.end += m_renderViewport.begin;
		
		//renderViewport.begin = m_renderOffset + viewport.begin * m_renderScale;
		//renderViewport.end = m_renderOffset + viewport.end * m_renderScale;
		
		RENDER_SYSTEM(m_serviceProvider)->setViewport( renderViewport );

		const mt::mat4f & viewMatrix = camera->getViewMatrix();
		RENDER_SYSTEM(m_serviceProvider)->setModelViewMatrix( viewMatrix );

		const mt::mat4f & projectionMatrix = camera->getProjectionMatrix();
		RENDER_SYSTEM(m_serviceProvider)->setProjectionMatrix( projectionMatrix );

		this->renderObjects_( _renderPass );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderObjects_( const RenderPass & _renderPass )
	{
		TVectorRenderObject::const_iterator it_begin = m_renderObjects.begin();
		std::advance( it_begin, _renderPass.beginRenderObject );

		TVectorRenderObject::const_iterator it_end = m_renderObjects.begin();
		std::advance( it_end, _renderPass.beginRenderObject + _renderPass.countRenderObject );

		for( ;it_begin != it_end; ++it_begin )
		{
			const RenderObject* renderObject = &(*it_begin);

			this->renderObject_( renderObject );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::addRenderObject2D( const RenderCameraInterface * _camera, const RenderMaterial* _material, const RenderTextureInterfacePtr * _textures, size_t _texturesNum,
										const Vertex2D* _vertices, size_t _verticesNum,
										ELogicPrimitiveType _type, size_t _indicesNum, IBHandle _ibHandle )
	{
        (void)_indicesNum; //TODO
        (void)_ibHandle; //TODO

		if( _camera == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D camera == NULL"
				);

			return;
		}

		if( _material == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D _material == NULL"
				);

			return;
		}

		if( m_currentRenderCamera != _camera )
		{
			m_currentRenderCamera = _camera;

			RenderPass pass;
			pass.beginRenderObject = m_renderObjects.size();
			pass.countRenderObject = 0;
			pass.camera = m_currentRenderCamera;

			m_renderPasses.push_back( pass );
			m_currentRenderPass = &m_renderPasses.back();
		}

        m_renderObjects.push_back( RenderObject() );
		RenderObject & ro = m_renderObjects.back();

		ro.material = _material;

		ro.textureStages = _texturesNum;

		ro.logicPrimitiveType = _type;

		ro.dipIndiciesNum = m_primitiveIndexStride[_type] * _verticesNum / m_primitiveVertexStride[_type];
		ro.ibHandle = m_ibHandle2D;
	
        if( _vertices == nullptr )
        {
            return;
        }

		ro.vertexData = _vertices;
		ro.verticesNum = _verticesNum;

		ro.baseVertexIndex = 0;

		for( size_t i = 0; i != _texturesNum; ++i )
		{
			if( _textures == NULL )
			{
				ro.textures[i] = 0;
			}
			else
			{
				ro.textures[i] = _textures[i];
			}		 
		}

		switch( _type )
		{
		case LPT_QUAD:
		//case LPT_TRIANGLE:
			ro.primitiveType = PT_TRIANGLELIST;

            if( m_debugMode == true )
            {
                this->calcQuadSquare_( _vertices, _verticesNum );
            }
			break;
		case LPT_LINE:
		//case LPT_RECTANGLE:
			ro.primitiveType = PT_LINESTRIP;
			break;
		default:
			ro.primitiveType = PT_TRIANGLELIST;
		}

		++m_currentRenderPass->countRenderObject;
	}
	//////////////////////////////////////////////////////////////////////////
	VBHandle RenderEngine::createVertexBuffer( const Vertex2D * _buffer, size_t _count )
	{
		VBHandle vbHandle = RENDER_SYSTEM(m_serviceProvider)
            ->createVertexBuffer( _count, sizeof(Vertex2D) );

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
		IBHandle ibHandle = RENDER_SYSTEM(m_serviceProvider)
            ->createIndexBuffer(_count);

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
			LOGGER_ERROR(m_serviceProvider)( "RenderEngine: failed to release vertex buffer" 
                );

			return;
		}

		m_vertexBuffer.erase( it_found );

		RENDER_SYSTEM(m_serviceProvider)
            ->releaseVertexBuffer( _handle );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseIndicesBuffer( IBHandle _handle )
	{
		TVectorIndexBuffer::iterator it_found = std::find( m_indexBuffer.begin(), m_indexBuffer.end(), _handle );

		if( it_found == m_indexBuffer.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "RenderEngine: failed to release index buffer" 
                );

			return;
		}

		m_indexBuffer.erase( it_found );

		RENDER_SYSTEM(m_serviceProvider)
            ->releaseIndexBuffer( _handle );
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::updateVertexBuffer( VBHandle _handle, const Vertex2D * _buffer, size_t _count  )
	{
		void * vbuffer = RENDER_SYSTEM(m_serviceProvider)->lockVertexBuffer( 
			_handle
            , 0
            , _count * sizeof(Vertex2D)
            , LOCK_DISCARD 
            );

		if( vbuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine: failed to lock vertex buffer"
                );

			return false;
		}

		std::copy( _buffer + 0, _buffer + _count, static_cast<Vertex2D*>(vbuffer) );

		if( RENDER_SYSTEM(m_serviceProvider)->unlockVertexBuffer( _handle ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "RenderEngine: failed to unlock vertex buffer" 
                );

			return false;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::updateIndicesBuffer( IBHandle _handle, const unsigned short * _buffer, size_t _count )
	{
		uint16* ibuffer = RENDER_SYSTEM(m_serviceProvider)->lockIndexBuffer( _handle );

		if( ibuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::updateIndicesBuffer: failed to lock vertex buffer"
                );

			return false;
		}

		std::copy( _buffer + 0, _buffer + _count, ibuffer );

		if( RENDER_SYSTEM(m_serviceProvider)->unlockIndexBuffer( _handle ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::updateIndicesBuffer: failed to unlock vertex buffer"
                );

			return false;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderEngine::makeBatch_( size_t _offset )
	{
		size_t vbPos = _offset;

		for( TVectorRenderPass::iterator 
			it = m_renderPasses.begin(), 
			it_end = m_renderPasses.end();
		it != it_end;
		++it )
		{
			RenderPass * pass = &(*it);

			vbPos += this->batchRenderObjects_( pass, vbPos );
		}

		return vbPos;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::makeBatches_( bool & _overflow )
	{		
		m_vbPos = 0;

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

		void * vData = RENDER_SYSTEM(m_serviceProvider)->lockVertexBuffer( 
			m_vbHandle2D
            , m_vbPos * sizeof(Vertex2D)
            , vertexDataSize * sizeof(Vertex2D)
            , lockFlags 
			);

		if( vData == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::makeBatches_: failed to lock vertex buffer"
                );

			return false;
		}

		size_t offset = 0;
		
		Vertex2D * vertexBuffer = static_cast<Vertex2D *>(vData);

		for( TVectorRenderPass::iterator 
			it = m_renderPasses.begin(), 
			it_end = m_renderPasses.end();
			it != it_end;
			++it )
		{
			RenderPass * pass = &(*it);
			
			offset = this->insertRenderObjects_( pass, vertexBuffer, offset );
		}

		if( RENDER_SYSTEM(m_serviceProvider)->unlockVertexBuffer( m_vbHandle2D ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Error: failed to unlock vertex buffer"
                );

			return false;
		}

		_overflow = false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline static bool s_checkForBatch( const RenderObject * _prev, const RenderObject * _next )
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
			RenderImageInterfacePtr prevImage = _prev->textures[i]->getImage();
			RenderImageInterfacePtr nextImage = _next->textures[i]->getImage();

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
	size_t RenderEngine::batchRenderObjects_( RenderPass * _renderPass, size_t _startVertexPos )
	{
		RenderObject* batchedRO = NULL;
		size_t verticesNum = _startVertexPos;

		TVectorRenderObject::iterator it_begin = m_renderObjects.begin();
		std::advance( it_begin, _renderPass->beginRenderObject );

		TVectorRenderObject::iterator it_end = m_renderObjects.begin();
		std::advance( it_end, _renderPass->beginRenderObject + _renderPass->countRenderObject );

		for( ; it_begin != it_end; ++it_begin )
		{
			RenderObject * renderObject = &(*it_begin);

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
			_renderObject->material = 0;
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
	size_t RenderEngine::insertRenderObjects_( RenderPass * _renderPass, Vertex2D * _vertexBuffer, size_t _offset )
	{
		TVectorRenderObject::iterator it_begin = m_renderObjects.begin();
		std::advance( it_begin, _renderPass->beginRenderObject );

		TVectorRenderObject::iterator it_end = m_renderObjects.begin();
		std::advance( it_end, _renderPass->beginRenderObject + _renderPass->countRenderObject );

		for( ; it_begin != it_end; ++it_begin )
		{
			RenderObject * ro = &(*it_begin);

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
			LOGGER_ERROR(m_serviceProvider)("Warning: vertex buffer overflow"
                );

			_renderObject->verticesNum = m_maxPrimitiveVertices2D - m_vbPos;
			//return;
		}

		_renderObject->startIndex  = indexStart + m_vbPos / vertexStride * indexStride;
		_renderObject->minIndex = (_renderObject->startIndex - indexStart) / indexStride * vertexStride;

		assert( _renderObject->startIndex + _renderObject->dipIndiciesNum <= m_maxIndexCount );

		//Vertex2D * vertexBuffer = _vertexBuffer + _offset * sizeof(Vertex2D)
		Vertex2D * offsetVertexBuffer = _vertexBuffer + _offset;
		//std::copy( _renderObject->vertexData, _renderObject->vertexData + _renderObject->verticesNum, offsetVertexBuffer );
        memcpy( offsetVertexBuffer, _renderObject->vertexData, _renderObject->verticesNum * sizeof(Vertex2D) );

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

			LOGGER_WARNING(m_serviceProvider)("Warning: vertex buffer overflow"
                );

			////vertexDataSize = m_maxPrimitiveVertices2D;
			//setRenderSystemDefaults_( m_maxQuadCount * 2 );
			//recreate2DBuffers_( m_maxIndexCount );
			//restoreRenderSystemStates_();
			//prepare2D_();
			return;
		}

		m_debugInfo.dips = 0;

		this->renderPasses_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::prepare2D_()
	{
		if( m_currentVBHandle != m_vbHandle2D )
		{
			m_currentVBHandle = m_vbHandle2D;

			RENDER_SYSTEM(m_serviceProvider)
                ->setVertexBuffer( m_currentVBHandle );
		}

		if( m_currentVertexDeclaration != Vertex2D_declaration )
		{
			m_currentVertexDeclaration = Vertex2D_declaration;

			RENDER_SYSTEM(m_serviceProvider)
                ->setVertexDeclaration( sizeof(Vertex2D), m_currentVertexDeclaration );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::prepare3D_()
	{		
		if( m_currentVertexDeclaration != Vertex3D_declaration )
		{
			m_currentVertexDeclaration = Vertex3D_declaration;

			RENDER_SYSTEM(m_serviceProvider)
                ->setVertexDeclaration( sizeof(Vertex3D), m_currentVertexDeclaration );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::refillIndexBuffer2D_( size_t & _maxVertices )
	{
		uint16* ibuffer = RENDER_SYSTEM(m_serviceProvider)
            ->lockIndexBuffer( m_ibHandle2D );

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
		maxVertices = (std::min)( maxVertices, vertexCount );
		// LINES
		vertexCount = 0;
		for( size_t i = m_primitiveIndexStart[LPT_LINE];
			i + m_primitiveIndexStride[LPT_LINE] < m_maxIndexCount;
			i += 1, vertexCount += 1 )
		{
			ibuffer[i+0] = 0 + vertexCount;
		}

		maxVertices = (std::min)( maxVertices, vertexCount );
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

		maxVertices = (std::min)( maxVertices, vertexCount );

		if( RENDER_SYSTEM(m_serviceProvider)->unlockIndexBuffer( m_ibHandle2D ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::refillIndexBuffer2D_: failed to unlock index buffer"
                );

            return false;
		}

        _maxVertices = maxVertices;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::recreate2DBuffers_( size_t _maxIndexCount )
	{
		if( m_ibHandle2D != 0 )
		{
			RENDER_SYSTEM(m_serviceProvider)
                ->releaseIndexBuffer( m_ibHandle2D );

			if( m_currentIBHandle == m_ibHandle2D )
			{
				m_currentIBHandle = 0;
			}

			m_ibHandle2D = 0;
		}

		m_ibHandle2D = RENDER_SYSTEM(m_serviceProvider)
            ->createIndexBuffer( _maxIndexCount );

		if( m_ibHandle2D == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "RenderEngine::recreate2DBuffers_: can't create index buffer for %d indicies"
				, m_maxIndexCount 
				);

			return false;
		}

		if( refillIndexBuffer2D_( m_maxPrimitiveVertices2D ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "RenderEngine::recreate2DBuffers_: can't refill index buffer"
                );

            return false;
        }

		if( m_vbHandle2D != 0 )
		{
			RENDER_SYSTEM(m_serviceProvider)
                ->releaseVertexBuffer( m_vbHandle2D );

			if( m_currentVBHandle == m_vbHandle2D )
			{
				m_currentVBHandle = 0;
			}

			m_vbHandle2D = 0;
		}

		m_vbHandle2D = RENDER_SYSTEM(m_serviceProvider)
            ->createVertexBuffer( m_maxPrimitiveVertices2D, sizeof(Vertex2D) );

		if( m_vbHandle2D == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "RenderEngine::recreate2DBuffers_: can't create index buffer for %d indicies"
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

        for( int i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
        {
            RenderTextureStage & stage = m_currentTextureStage[i];

            stage.addressU = TAM_CLAMP;
            stage.addressV = TAM_CLAMP;

            stage.colorOp = TOP_DISABLE;
            stage.colorArg1 = TARG_TEXTURE;
            stage.colorArg2 = TARG_DIFFUSE;

            stage.alphaOp = TOP_DISABLE;
            stage.colorArg1 = TARG_TEXTURE;
            stage.colorArg2 = TARG_DIFFUSE;

            stage.filter = TF_LINEAR;
        }

		std::fill_n( m_currentTexturesID, MENGE_MAX_TEXTURE_STAGES, 0 );

		m_renderObjects.reserve(1000);
		m_renderPasses.reserve(100);
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::enableTextureFiltering( bool _enable )
	{
		m_textureFiltering = _enable;
		
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
			RENDER_SYSTEM(m_serviceProvider)
                ->setVSync( m_vsync );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::getVSync() const
	{
		return m_vsync;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::clear( uint32 _color )
	{
		RENDER_SYSTEM(m_serviceProvider)
            ->clear( _color );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderTargetTexture( const RenderTextureInterfacePtr & _texture, bool _clear )
	{
		RenderImageInterfacePtr image = _texture->getImage();
		
		RENDER_SYSTEM(m_serviceProvider)
            ->setRenderTarget( image, _clear );

		this->restoreRenderSystemStates_();
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr RenderEngine::createRenderTargetTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
	{
        std::size_t pow2_width = _width;
        std::size_t pow2_height = _height;

        if( ( _width & ( _width - 1 ) ) != 0 || ( _height & ( _height - 1 ) ) != 0 )
        {
            pow2_width = s_firstPO2From( _width );
            pow2_height = s_firstPO2From( _height );
        }

		RenderImageInterfacePtr image = RENDER_SYSTEM(m_serviceProvider)
            ->createRenderTargetImage( pow2_width, pow2_height, _channels, _format );

		if( image == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "Error: (RenderEngine::createRenderTargetImage) RenderSystem couldn't create RenderTargetTexture %dx%d %d"
				, pow2_width
				, pow2_height 
                , _channels
				);

			return nullptr;
		}

		//printf("m_debugInfo.textureMemory %d %f\n", m_debugInfo.textureCount, float(m_debugInfo.textureMemory) / (1024.f * 1024.f));

        size_t id = ++m_idEnumerator;

        RenderTexture * texture = m_factoryRenderTexture.createObjectT();
        texture->initialize( image, _width, _height, _channels, id, this );

        size_t memroy_size = texture->getMemoryUse();

        m_debugInfo.textureMemory += memroy_size;
        ++m_debugInfo.textureCount;

        LOGGER_INFO(m_serviceProvider)( "RenderEngine::createRenderTargetTexture creating render target texture %dx%d %d memory %d:%d"
            , pow2_width
            , pow2_height 
            , _channels 
            , memroy_size
            , m_debugInfo.textureMemory
            );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setSeparateAlphaBlendMode()
	{
		RENDER_SYSTEM(m_serviceProvider)
            ->setSeparateAlphaBlendMode();
	}
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::enableDebugMode( bool _enable )
    {
        m_debugMode = _enable;
    }
	//////////////////////////////////////////////////////////////////////////
	const Viewport & RenderEngine::getRenderViewport() const
	{
		return m_renderViewport;
	}
    //////////////////////////////////////////////////////////////////////////
    static double s_calcTriangleSquare( const Vertex2D & _v1, const Vertex2D & _v2, const Vertex2D & _v3 )
    {
        const mt::vec3f & p1 = _v1.pos;
        const mt::vec3f & p2 = _v2.pos;
        const mt::vec3f & p3 = _v3.pos;

        float a = mt::length_v3_v3( p1, p2 );
        float b = mt::length_v3_v3( p2, p3 );
        float c = mt::length_v3_v3( p3, p1 );

        double p = (a + b + c) * 0.5f;

        double S = sqrt( p * (p-a) * (p-b) * (p-c) );

        return S;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderEngine::calcQuadSquare_( const Vertex2D * _vertex, size_t _num )
    {
        for( size_t i = 0; i != (_num / 4); ++i )
        {
            m_debugInfo.fillrate += s_calcTriangleSquare( _vertex[i + 0], _vertex[i + 1], _vertex[i + 2] );
            m_debugInfo.fillrate += s_calcTriangleSquare( _vertex[i + 0], _vertex[i + 2], _vertex[i + 3] );

            m_debugInfo.triangle += 2;
        }        

        m_debugInfo.object += 1;
    }
}
