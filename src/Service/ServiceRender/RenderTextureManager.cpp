#   include "RenderTextureManager.h"

#   include "Interface/FileSystemInterface.h"
#   include "Interface/WatchdogInterface.h"
#   include "Interface/StringizeInterface.h"
#	include "Interface/PrefetcherInterface.h"
#	include "Interface/GraveyardInterface.h"
#	include "Interface/ConfigInterface.h"

#   include "RenderTexture.h"
#   include "DecoderRenderImageProvider.h"

#   include "Factory/FactoryPool.h"

#	include "stdex/memorycopy.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderTextureService, Menge::RenderTextureManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    RenderTextureManager::RenderTextureManager()
        : m_textureEnumerator(0)
		, m_supportA8(false)
        , m_supportL8( false )
		, m_supportR8G8B8(false)
		, m_supportNonPow2(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureManager::~RenderTextureManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureManager::_initialize()
    {
		m_supportA8 = RENDER_SYSTEM(m_serviceProvider)
			->supportTextureFormat( PF_A8 );

        m_supportL8 = RENDER_SYSTEM( m_serviceProvider )
            ->supportTextureFormat( PF_L8 );

		m_supportR8G8B8 = RENDER_SYSTEM(m_serviceProvider)
			->supportTextureFormat( PF_R8G8B8 );

		m_supportNonPow2 = RENDER_SYSTEM( m_serviceProvider )
			->supportTextureNonPow2();

        m_factoryRenderTexture = Helper::makeFactoryPool<RenderTexture, 128>( m_serviceProvider, this, &RenderTextureManager::onRenderTextureDestroy_ );

        m_factoryDecoderRenderImageProvider = new FactoryPool<DecoderRenderImageProvider, 128>( m_serviceProvider );
		
		return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::_finalize()
    {   
		for( uint32_t i = 0; i != MENGINE_TEXTURE_MANAGER_HASH_SIZE; ++i )
		{
			TMapRenderTextureEntry & textures = m_textures[i];

			for( TMapRenderTextureEntry::iterator
				it = textures.begin(),
				it_end = textures.end();
			it != it_end;
			++it )
			{
				RenderTextureInterface * texture = it->second;

				texture->release();
			}

			textures.clear();
		}

        m_factoryRenderTexture = nullptr;
		m_factoryDecoderRenderImageProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureManager::hasTexture( const ConstString& _pakName, const FilePath & _fileName, RenderTextureInterfacePtr * _texture ) const
    {
        const TMapRenderTextureEntry & textures = this->getHashEntry_(_fileName);

		TMapRenderTextureEntry::const_iterator it_found = textures.find( std::make_pair( _pakName, _fileName ) );

		if( it_found == textures.end() )
		{
			if( _texture != nullptr )
			{
				*_texture = nullptr;
			}

			return false;
		}

		const RenderTexturePtr & texture = it_found->second;

		if( _texture != nullptr )
		{
			*_texture = texture;
		}
				
		return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureManager::getTexture( const ConstString & _pakName, const FilePath & _fileName ) const
    {
        const TMapRenderTextureEntry & textures = this->getHashEntry_(_fileName);

		TMapRenderTextureEntry::const_iterator it_found = textures.find( std::make_pair( _pakName, _fileName ) );

        if( it_found == textures.end() )
        {
            return nullptr;
        }

		const RenderTexturePtr & texture = it_found->second;
				
        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr RenderTextureManager::createTexture( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
	{
		uint32_t HWMipmaps = _mipmaps;
		uint32_t HWWidth = _width;
		uint32_t HWHeight = _height;
		uint32_t HWChannels = _channels;
		uint32_t HWDepth = _depth;

		PixelFormat HWFormat = _format;

		this->updateImageParams_( HWWidth, HWHeight, HWChannels, HWDepth, HWFormat );

		RenderImageInterfacePtr image = RENDER_SYSTEM( m_serviceProvider )
			->createImage( HWMipmaps, HWWidth, HWHeight, HWChannels, HWDepth, HWFormat );

		if( image == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderTextureManager::createTexture_ invalid create image %ux%u"
				, HWWidth
				, HWHeight
				);

			return nullptr;
		}

		RenderTextureInterfacePtr texture = this->createRenderTexture( image, _width, _height );

		if( texture == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderTextureManager::createTexture_ invalid create render texture %ux%u"
				, _width
				, _height
				);

			return nullptr;
		}

		return texture;
	}
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureManager::createDynamicTexture( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
    {
		uint32_t HWWidth = _width;
		uint32_t HWHeight = _height;
		uint32_t HWChannels = _channels;
		uint32_t HWDepth = _depth;
		PixelFormat HWFormat = _format;

		this->updateImageParams_( HWWidth, HWHeight, HWChannels, HWDepth, HWFormat );

        RenderImageInterfacePtr image = RENDER_SYSTEM(m_serviceProvider)
            ->createDynamicImage( HWWidth, HWHeight, HWChannels, HWDepth, HWFormat );

        if( image == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::createDynamicTexture couldn't create image %dx%d channels %d"
                , HWWidth
                , HWHeight
                , HWChannels
                );

            return nullptr;
        }

        RenderTextureInterfacePtr texture = this->createRenderTexture( image, _width, _height );

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureManager::saveImage( const RenderTextureInterfacePtr & _texture, const ConstString & _fileGroupName, const ConstString & _codecName, const FilePath & _fileName )
    {
        OutputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( _fileGroupName, _fileName );

        if( stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::saveImage : can't create file '%s' '%s'"
                , _fileGroupName.c_str()
                , _fileName.c_str() 
                );

            return false;
        }

        ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE(m_serviceProvider)
            ->createEncoderT<ImageEncoderInterfacePtr>( _codecName );

        if( imageEncoder == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::saveImage : can't create encoder for filename '%s'"
                , _fileName.c_str() 
                );

            return false;
        }

		if( imageEncoder->initialize( stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::saveImage : can't initialize encoder for filename '%s'"
				, _fileName.c_str() 
				);

			return false;
		}

        ImageCodecDataInfo dataInfo;
        //dataInfo.format = _image->getHWPixelFormat();
        dataInfo.width = _texture->getWidth();
        dataInfo.height = _texture->getHeight();		
        dataInfo.depth = 1;
        dataInfo.mipmaps = 1;

		const RenderImageInterfacePtr & image = _texture->getImage();
        
        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = dataInfo.width;
        rect.bottom = dataInfo.height;

		size_t pitch = 0;
        void * buffer = image->lock( &pitch, 0, rect, true );

		if( buffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::saveImage : can't lock texture '%s'"
				, _fileName.c_str() 
				);

			return false;
		}

        ImageCodecOptions options;		

        options.pitch = pitch;
        options.channels = 4;
        //options.flags |= DF_CUSTOM_PITCH;

        if( imageEncoder->setOptions( &options ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::saveImage : invalid optionize '%s'"
				, _fileName.c_str() 
				);

			image->unlock( 0 );

			return false;
		}

		size_t bufferSize = options.pitch * dataInfo.height;

		size_t bytesWritten = imageEncoder->encode( buffer, bufferSize, &dataInfo );

		image->unlock( 0 );

        if( bytesWritten == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::saveImage : Error while encoding image data"
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::visitTexture( VisitorRenderTextureInterface * _visitor ) const
    {
		for( uint32_t i = 0; i != MENGINE_TEXTURE_MANAGER_HASH_SIZE; ++i )
		{
			const TMapRenderTextureEntry & textures = m_textures[i];

			for( TMapRenderTextureEntry::const_iterator
				it = textures.begin(),
				it_end = textures.end();
			it != it_end;
			++it )
			{
				const RenderTexturePtr & texture = it->second;

				const RenderTexture * texture_ptr = texture.get();

				_visitor->visitRenderTexture( texture_ptr );
			}
		}
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::cacheFileTexture( const ConstString& _pakName, const FilePath& _fileName, const RenderTextureInterfacePtr & _texture )
    {
		_texture->setCategory( _pakName );
        _texture->setFileName( _fileName );

        TMapRenderTextureEntry & textures = this->getHashEntry_(_fileName);
		
		RenderTextureInterface * texture_ptr = _texture.get();

		textures.insert( std::make_pair( std::make_pair( _pakName, _fileName ), texture_ptr ) );

        LOGGER_INFO(m_serviceProvider)( "RenderTextureManager::cacheFileTexture cache texture %s:%s"
			, _pakName.c_str()
            , _fileName.c_str()
            );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureManager::loadTexture( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecName )
    {
        const TMapRenderTextureEntry & textures = this->getHashEntry_(_fileName);

		TMapRenderTextureEntry::const_iterator it_found = textures.find( std::make_pair( _pakName, _fileName ) );

		if( it_found != textures.end() )
        {
			const RenderTexturePtr & texture = it_found->second;

            return texture;
        }

		if( SERVICE_EXIST( m_serviceProvider, Menge::GraveyardInterface ) == true )
		{
			RenderTextureInterfacePtr resurrect_texture = GRAVEYARD_SERVICE( m_serviceProvider )
				->resurrectTexture( _pakName, _fileName );

			if( resurrect_texture != nullptr )
			{
				this->cacheFileTexture( _pakName, _fileName, resurrect_texture );

				return resurrect_texture;
			}
		}

        DecoderRenderImageProviderPtr imageProvider = m_factoryDecoderRenderImageProvider->createObject();

        imageProvider->setServiceProvider( m_serviceProvider );
        imageProvider->initialize( _pakName, _fileName, _codecName );

		RenderImageLoaderInterfacePtr imageLoader = imageProvider->getLoader();

		RenderImageDesc imageDesc = imageLoader->getImageDesc();
		        		
		RenderTextureInterfacePtr new_texture = this->createTexture( imageDesc.mipmaps, imageDesc.width, imageDesc.height, imageDesc.channels, imageDesc.depth, imageDesc.format );

		if( new_texture == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderTextureManager::loadTexture create texture '%s:%s' codec '%s'"				
				, _pakName.c_str()
				, _fileName.c_str()
				, _codecName.c_str()
				);

			return nullptr;
		}

		RenderImageInterfacePtr image = new_texture->getImage();

		if( image == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderTextureManager::loadTexture invalid get image"
				);

			return nullptr;
		}

		Rect rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = image->getHWWidth();
		rect.bottom = image->getHWHeight();

		size_t pitch = 0;
		void * textureBuffer = image->lock( &pitch, 0, rect, false );

		if( textureBuffer == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderTextureManager::createTexture Invalid lock mipmap %d rect %d:%d-%d:%d"
				, 0
				, rect.left
				, rect.top
				, rect.right
				, rect.bottom
				);

			return nullptr;
		}

		if( imageLoader->load( textureBuffer, pitch ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("RenderTextureManager::createTexture Invalid decode image"
				);

			image->unlock( 0 );

			return nullptr;
		}

		image->unlock( 0 );

		image->setRenderImageProvider( imageProvider );
        
        this->cacheFileTexture( _pakName, _fileName, new_texture );

        return new_texture;
    }
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTextureManager::getImageMemoryUse( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) const
	{
		uint32_t HWWidth = _width;
		uint32_t HWHeight = _height;
		uint32_t HWChannels = _channels;
		uint32_t HWDepth = _depth;
		PixelFormat HWFormat = _format;

		this->updateImageParams_( HWWidth, HWHeight, HWChannels, HWDepth, HWFormat );

		size_t memoryUse = Helper::getTextureMemorySize( HWWidth, HWHeight, HWChannels, HWDepth, HWFormat );

		return memoryUse;
	}
	//////////////////////////////////////////////////////////////////////////	
	void RenderTextureManager::updateImageParams_( uint32_t & _width, uint32_t & _height, uint32_t & _channels, uint32_t & _depth, PixelFormat & _format ) const
	{
		if( (( _width & ( _width - 1 ) ) != 0 || ( _height & ( _height - 1 ) ) != 0) /*&& m_supportNonPow2 == false*/ )
		{
			_width = Helper::getTexturePOW2( _width );
			_height = Helper::getTexturePOW2( _height );
		}

		switch( _format )
		{
		case PF_UNKNOWN:
			{
				if( _channels == 1 )
				{
					if( m_supportA8 == true )
					{
						_format = PF_A8;
					}
					else
					{
                        _format = PF_A8R8G8B8;

                        _channels = 4;
					}
				}
				else if( _channels == 3 )
				{
					if( m_supportR8G8B8 == true )
					{
						_format = PF_R8G8B8;
					}
					else
					{
						_format = PF_X8R8G8B8;
						_channels = 4;
					}
				}
				else if( _channels == 4 )
				{
                    _format = PF_A8R8G8B8;
				}
			}break;
        case PF_A8:
            {
                if( _channels == 1 )
                {
                    if( m_supportA8 == true )
                    {
                        _format = PF_A8;
                    }
                    else
                    {
                        _format = PF_A8R8G8B8;

                        _channels = 4;
                    }
                }
            }break;
		default:
			{
			}break;
		}

        if( _channels == 0 )
        {
            switch( _format )
            {
            case PF_A8:
                {
                    if( m_supportA8 == true )
                    {
                        _channels = 1;
                    }
                    else
                    {
                        _format = PF_A8R8G8B8;

                        _channels = 4;
                    }
                }break;
            case PF_A8B8G8R8:
                {
                    _channels = 4;
                }break;
            case PF_A8R8G8B8:
                {
                    _channels = 4;
                }break;
            default:
                {
                }break;
            }
        }

		(void)_depth; //ToDo
	}
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureManager::onRenderTextureDestroy_( RenderTextureInterface * _texture )
    {
		const ConstString & category = _texture->getCategory();
		const FilePath & fileName = _texture->getFileName();

        TMapRenderTextureEntry & textures = this->getHashEntry_(fileName);

		textures.erase( std::make_pair( category, fileName ) );
				
		if( SERVICE_EXIST( m_serviceProvider, Menge::GraveyardInterface ) == true )
		{
			GRAVEYARD_SERVICE( m_serviceProvider )
				->buryTexture( _texture );
		}

        _texture->release();
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr RenderTextureManager::createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height )
	{
		RenderTexturePtr texture = m_factoryRenderTexture->createObject();

		texture->setServiceProvider(m_serviceProvider);

        uint32_t id = ++m_textureEnumerator;

		texture->initialize( id, _image, _width, _height );

		return texture;
	}
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::imageQuality( const RenderTextureInterfacePtr & _texture, void * _textureBuffer, size_t _texturePitch )
    {
		const RenderImageInterfacePtr & image = _texture->getImage();

		PixelFormat hwPixelFormat = image->getHWPixelFormat();

		if( hwPixelFormat != PF_A8R8G8B8 &&
			hwPixelFormat != PF_A8B8G8R8 &&
			hwPixelFormat != PF_B8G8R8A8 &&
			hwPixelFormat != PF_R8G8B8A8 &&
			hwPixelFormat != PF_X8R8G8B8 &&
			hwPixelFormat != PF_X8B8G8R8 )
		{
			return;
		}

		uint32_t width = _texture->getWidth();
		uint32_t height = _texture->getHeight();

		uint32_t hwWidth = image->getHWWidth();
		uint32_t hwHeight = image->getHWHeight();
		
        // copy pixels on the edge for better image quality
        if( hwWidth > width )
        {
            LOGGER_INFO(m_serviceProvider)("imageQuality_ width %d - %d"
                , width
                , hwWidth
                );

            unsigned char * image_data = static_cast<unsigned char *>(_textureBuffer);
            size_t pixel_size = _texturePitch / hwWidth;

            for( uint32_t j = 0; j != height; ++j )
            {
     //           std::copy( image_data + (width - 1) * pixel_size
					//, image_data + width * pixel_size
					//, image_data + width * pixel_size );

				stdex::memorycopy( image_data, width * pixel_size, image_data + (width - 1) * pixel_size, pixel_size );

                image_data += _texturePitch;
            }
        }

        if( hwHeight > height )
        {
            LOGGER_INFO(m_serviceProvider)("imageQuality_ height %d - %d"
                , height
                , hwHeight
                );

            unsigned char * image_data = static_cast<unsigned char *>(_textureBuffer);

			stdex::memorycopy( image_data, height * _texturePitch, image_data + (height - 1) * _texturePitch, _texturePitch );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureManager::TMapRenderTextureEntry & RenderTextureManager::getHashEntry_(const ConstString & _fileName)
    {
        ConstString::hash_type hash = _fileName.hash();
        uint32_t table = (uint32_t)hash % MENGINE_TEXTURE_MANAGER_HASH_SIZE;
        TMapRenderTextureEntry & textures = m_textures[table];

        return textures;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTextureManager::TMapRenderTextureEntry & RenderTextureManager::getHashEntry_(const ConstString & _fileName) const
    {
        ConstString::hash_type hash = _fileName.hash();
        uint32_t table = (uint32_t)hash % MENGINE_TEXTURE_MANAGER_HASH_SIZE;
        const TMapRenderTextureEntry & textures = m_textures[table];

        return textures;
    }
}