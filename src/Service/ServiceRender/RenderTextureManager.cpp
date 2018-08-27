#include "RenderTextureManager.h"

#include "Interface/FileSystemInterface.h"
#include "Interface/WatchdogInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/PrefetcherInterface.h"
#include "Interface/GraveyardInterface.h"
#include "Interface/ConfigInterface.h"

#include "RenderTexture.h"
#include "DecoderRenderImageProvider.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"

#include "stdex/memorycopy.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderTextureService, Mengine::RenderTextureManager );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderTextureManager::RenderTextureManager()
        : m_textureEnumerator( 0 )
        , m_supportA8( false )
        , m_supportL8( false )
        , m_supportR8G8B8( false )
        , m_supportNonPow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureManager::~RenderTextureManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureManager::_initializeService()
    {
        m_supportA8 = RENDER_SYSTEM()
            ->supportTextureFormat( PF_A8 );

        m_supportL8 = RENDER_SYSTEM()
            ->supportTextureFormat( PF_L8 );

        m_supportR8G8B8 = RENDER_SYSTEM()
            ->supportTextureFormat( PF_R8G8B8 );

        m_supportNonPow2 = RENDER_SYSTEM()
            ->supportTextureNonPow2();

        m_factoryRenderTexture = Helper::makeFactoryPoolWithListener<RenderTexture, 128>( this, &RenderTextureManager::onRenderTextureDestroy_ );

        m_factoryDecoderRenderImageProvider = new FactoryPool<DecoderRenderImageProvider, 128>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::_finalizeService()
    {
        for( uint32_t i = 0; i != MENGINE_TEXTURE_MANAGER_HASH_SIZE; ++i )
        {
            MapRenderTextureEntry & textures = m_textures[i];

            for( MapRenderTextureEntry::iterator
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

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderTexture );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDecoderRenderImageProvider );

        m_factoryRenderTexture = nullptr;
        m_factoryDecoderRenderImageProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureManager::hasTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, RenderTextureInterfacePtr * _texture ) const
    {
        const MapRenderTextureEntry & textures = this->getHashEntry_( _fileName );

        MapRenderTextureEntry::const_iterator it_found = textures.find( std::make_pair( _fileGroup->getName(), _fileName ) );

        if( it_found == textures.end() )
        {
            if( _texture != nullptr )
            {
                *_texture = nullptr;
            }

            return false;
        }

        const RenderTextureInterface * texture = it_found->second;

        if( _texture != nullptr )
        {
            *_texture = texture;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureManager::getTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _fileName ) const
    {
        const MapRenderTextureEntry & textures = this->getHashEntry_( _fileName );

        MapRenderTextureEntry::const_iterator it_found = textures.find( std::make_pair( _fileGroup->getName(), _fileName ) );

        if( it_found == textures.end() )
        {
            return RenderTextureInterfacePtr::none();
        }

        const RenderTextureInterface * texture = it_found->second;

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

        RenderImageInterfacePtr image = RENDER_SYSTEM()
            ->createImage( HWMipmaps, HWWidth, HWHeight, HWChannels, HWDepth, HWFormat );

        if( image == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::createTexture_ invalid create image %ux%u"
                , HWWidth
                , HWHeight
            );

            return nullptr;
        }

        RenderTextureInterfacePtr texture = this->createRenderTexture( image, _width, _height );

        if( texture == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::createTexture_ invalid create render texture %ux%u"
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

        RenderImageInterfacePtr image = RENDER_SYSTEM()
            ->createDynamicImage( HWWidth, HWHeight, HWChannels, HWDepth, HWFormat );

        if( image == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::createDynamicTexture couldn't create image %dx%d channels %d"
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
    bool RenderTextureManager::saveImage( const RenderTextureInterfacePtr & _texture, const FileGroupInterfacePtr & _fileGroup, const ConstString & _codecName, const FilePath & _fileName )
    {
        OutputStreamInterfacePtr stream = FILE_SERVICE()
            ->openOutputFile( _fileGroup, _fileName );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::saveImage : can't create file '%s' '%s'"
                , _fileGroup->getName().c_str()
                , _fileName.c_str()
            );

            return false;
        }

        ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE()
            ->createEncoderT<ImageEncoderInterfacePtr>( _codecName );

        if( imageEncoder == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::saveImage : can't create encoder for filename '%s'"
                , _fileName.c_str()
            );

            return false;
        }

        if( imageEncoder->initialize( stream ) == false )
        {
            LOGGER_ERROR( "RenderTextureManager::saveImage : can't initialize encoder for filename '%s'"
                , _fileName.c_str()
            );

            return false;
        }

        ImageCodecDataInfo dataInfo;
        //dataInfo.format = _image->getHWPixelFormat();
        dataInfo.width = _texture->getWidth();
        dataInfo.height = _texture->getHeight();

        const RenderImageInterfacePtr & image = _texture->getImage();
        dataInfo.channels = image->getHWChannels();
        dataInfo.depth = 1;
        dataInfo.mipmaps = 1;

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = dataInfo.width;
        rect.bottom = dataInfo.height;

        size_t pitch = 0;
        void * buffer = image->lock( &pitch, 0, rect, true );

        if( buffer == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::saveImage : can't lock texture '%s'"
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
            LOGGER_ERROR( "RenderTextureManager::saveImage : invalid optionize '%s'"
                , _fileName.c_str()
            );

            image->unlock( 0, false );

            return false;
        }

        size_t bufferSize = options.pitch * dataInfo.height;

        size_t bytesWritten = imageEncoder->encode( buffer, bufferSize, &dataInfo );

        image->unlock( 0, true );

        if( bytesWritten == 0 )
        {
            LOGGER_ERROR( "RenderTextureManager::saveImage : Error while encoding image data"
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
            const MapRenderTextureEntry & textures = m_textures[i];

            for( MapRenderTextureEntry::const_iterator
                it = textures.begin(),
                it_end = textures.end();
                it != it_end;
                ++it )
            {
                const RenderTextureInterface * texture = it->second;

                _visitor->visitRenderTexture( texture );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureManager::cacheFileTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath& _fileName, const RenderTextureInterfacePtr & _texture )
    {
        _texture->setCategory( _fileGroup );
        _texture->setFileName( _fileName );

        MapRenderTextureEntry & textures = this->getHashEntry_( _fileName );

        RenderTextureInterface * texture_ptr = _texture.get();

        textures.emplace( std::make_pair( _fileGroup->getName(), _fileName ), texture_ptr );

        LOGGER_INFO( "RenderTextureManager::cacheFileTexture cache texture %s:%s"
            , _fileGroup->getName().c_str()
            , _fileName.c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureManager::loadTexture( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, const ConstString & _codecName )
    {
        const MapRenderTextureEntry & textures = this->getHashEntry_( _fileName );

        MapRenderTextureEntry::const_iterator it_found = textures.find( std::make_pair( _fileGroup->getName(), _fileName ) );

        if( it_found != textures.end() )
        {
            const RenderTextureInterface * texture = it_found->second;

            return texture;
        }

        if( SERVICE_EXIST( Mengine::GraveyardInterface ) == true )
        {
            RenderTextureInterfacePtr resurrect_texture = GRAVEYARD_SERVICE()
                ->resurrectTexture( _fileGroup, _fileName );

            if( resurrect_texture != nullptr )
            {
                this->cacheFileTexture( _fileGroup, _fileName, resurrect_texture );

                return resurrect_texture;
            }
        }

        //RenderTextureInterfacePtr atlas_texture;
        //if( this->findInAtlas( _pakName, _fileName, &atlas_texture ) == true )
        //{
        //	return atlas_texture;
        //}

        DecoderRenderImageProviderPtr imageProvider = m_factoryDecoderRenderImageProvider->createObject();

        if( imageProvider == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::loadTexture invalid create render image provider" );

            return nullptr;
        }

        imageProvider->initialize( _fileGroup, _fileName, _codecName );

        RenderImageLoaderInterfacePtr imageLoader = imageProvider->getLoader();

        if( imageLoader == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::loadTexture invalid get image loader" );

            return nullptr;
        }

        RenderImageDesc imageDesc = imageLoader->getImageDesc();

        RenderTextureInterfacePtr new_texture = this->createTexture( imageDesc.mipmaps, imageDesc.width, imageDesc.height, imageDesc.channels, imageDesc.depth, imageDesc.format );

        if( new_texture == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::loadTexture create texture '%s:%s' codec '%s'"
                , _fileGroup->getName().c_str()
                , _fileName.c_str()
                , _codecName.c_str()
            );

            return nullptr;
        }

        const RenderImageInterfacePtr & image = new_texture->getImage();

        if( image == nullptr )
        {
            LOGGER_ERROR( "RenderTextureManager::loadTexture invalid get image"
            );

            return nullptr;
        }

        if( imageLoader->load( image ) == false )
        {
            LOGGER_ERROR( "RenderTextureManager::createTexture Invalid decode image"
            );

            image->unlock( 0, false );

            return nullptr;
        }

        image->setRenderImageProvider( imageProvider );

        this->cacheFileTexture( _fileGroup, _fileName, new_texture );

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
        if( ((_width & (_width - 1)) != 0 || (_height & (_height - 1)) != 0) /*&& m_supportNonPow2 == false*/ )
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
        const FileGroupInterfacePtr & category = _texture->getCategory();
        const FilePath & fileName = _texture->getFileName();

        if( fileName.empty() == false )
        {
            MapRenderTextureEntry & textures = this->getHashEntry_( fileName );

            textures.erase( std::make_pair( category->getName(), fileName ) );

            if( SERVICE_EXIST( Mengine::GraveyardInterface ) == true )
            {
                GRAVEYARD_SERVICE()
                    ->buryTexture( _texture );
            }
        }

        _texture->release();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureManager::createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height )
    {
        RenderTexturePtr texture = m_factoryRenderTexture->createObject();

        uint32_t id = ++m_textureEnumerator;

        texture->initialize( id, _image, _width, _height );

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureManager::MapRenderTextureEntry & RenderTextureManager::getHashEntry_( const ConstString & _fileName )
    {
        ConstString::hash_type hash = _fileName.hash();
        uint32_t table = (uint32_t)hash % MENGINE_TEXTURE_MANAGER_HASH_SIZE;
        MapRenderTextureEntry & textures = m_textures[table];

        return textures;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTextureManager::MapRenderTextureEntry & RenderTextureManager::getHashEntry_( const ConstString & _fileName ) const
    {
        ConstString::hash_type hash = _fileName.hash();
        uint32_t table = (uint32_t)hash % MENGINE_TEXTURE_MANAGER_HASH_SIZE;
        const MapRenderTextureEntry & textures = m_textures[table];

        return textures;
    }
}