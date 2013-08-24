#   include "RenderTextureManager.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/WatchdogInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderTextureManager, Menge::RenderTextureServiceInterface, Menge::RenderTextureManager );
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
    RenderTextureManager::RenderTextureManager()
        : m_serviceProvider(nullptr)        
        , m_textureEnumerator(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureManager::~RenderTextureManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * RenderTextureManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureManager::initialize()
    {
        m_debugInfo.textureMemory = 0;
        m_debugInfo.textureCount = 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::finalize()
    {        
        m_textures.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureManager::hasTexture( const FilePath & _filename, RenderTextureInterfacePtr * _texture ) const
    {
		RenderTextureInterface * texture;
		bool result = m_textures.has( _filename, &texture );

		if( _texture != nullptr )
		{
			*_texture = texture;
		}

		return result;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureManager::getTexture( const FilePath& _filename )
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
    RenderTextureInterfacePtr RenderTextureManager::createTexture( size_t _imageWidth, size_t _imageHeight, size_t _imageChannels, PixelFormat _imageFormat, size_t _textureWidth, size_t _textureHeight )
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

        size_t id = ++m_textureEnumerator;

        RenderTexture * texture = m_factoryRenderTexture.createObjectT();
        texture->initialize( m_serviceProvider, image, _textureWidth, _textureHeight, _imageChannels, id, this );

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
    RenderTextureInterfacePtr RenderTextureManager::createDynamicTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
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

        size_t id = ++m_textureEnumerator;

        RenderTexture * texture = m_factoryRenderTexture.createObjectT();
        texture->initialize( m_serviceProvider, image, _width, _height, _channels, id, this );

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
    RenderTextureInterfacePtr RenderTextureManager::createRenderTargetTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
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

        size_t id = ++m_textureEnumerator;

        RenderTexture * texture = m_factoryRenderTexture.createObjectT();
        texture->initialize( m_serviceProvider, image, _width, _height, _channels, id, this );

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
    bool RenderTextureManager::saveImage( const RenderTextureInterfacePtr & _texture, const ConstString & _fileSystemName, const ConstString & _codecName, const FilePath & _filename )
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
            ->createEncoderT<ImageEncoderInterfacePtr>( _codecName, stream );

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
        dataInfo.mipmaps = 0;
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
    void RenderTextureManager::visitTexture( VisitorRenderTextureInterface * _visitor ) const
    {
        for( TMapTextures::const_iterator
            it = m_textures.begin(),
            it_end = m_textures.end();
        it != it_end;
        ++it )
        {
            const RenderTextureInterfacePtr & texture = it->second;

            _visitor->visitRenderTexture( texture );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::cacheFileTexture( const FilePath& _filename, const RenderTextureInterfacePtr & _texture )
    {
        _texture->setFileName( _filename );

        RenderTextureInterface * texture_ptr = _texture.get();
        m_textures.insert( _filename, texture_ptr );

        LOGGER_INFO(m_serviceProvider)( "RenderEngine::cacheFileTexture cache texture %s %d:%d"
            , _filename.c_str()
            , _texture->getHWWidth()
            , _texture->getHWHeight()
            );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureManager::loadTexture( const ConstString& _pakName, const FilePath & _filename, const ConstString& _codec, size_t _width, size_t _height )
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
    void RenderTextureManager::onRenderTextureRelease( const RenderTextureInterface * _texture )
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
    bool RenderTextureManager::loadTextureRectImageData( const RenderTextureInterfacePtr & _texture, const Rect & _rect, const ImageDecoderInterfacePtr & _imageDecoder )
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
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::imageQuality( const RenderTextureInterfacePtr & _texture, unsigned char * _textureBuffer, size_t _texturePitch )
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
    const RenderTextureDebugInfo & RenderTextureManager::getDebugInfo()
    {
        return m_debugInfo;
    }
}