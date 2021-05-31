#include "RenderTextureService.h"

#include "Interface/WatchdogServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/GraveyardServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "RenderTexture.h"
#include "DecoderRenderImageProvider.h"
#include "DecoderRenderImageLoader.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/PixelFormatHelper.h"

#include "stdex/memorycopy.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderTextureService, Mengine::RenderTextureService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderTextureService::RenderTextureService()
        : m_supportA8( false )
        , m_supportL8( false )
        , m_supportR8G8B8( false )
        , m_supportNonPow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureService::~RenderTextureService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureService::_initializeService()
    {
        m_supportA8 = RENDER_SYSTEM()
            ->supportTextureFormat( PF_A8 );

        m_supportL8 = RENDER_SYSTEM()
            ->supportTextureFormat( PF_L8 );

        m_supportR8G8B8 = RENDER_SYSTEM()
            ->supportTextureFormat( PF_R8G8B8 );

        m_supportNonPow2 = RENDER_SYSTEM()
            ->supportTextureNonPow2();

        m_factoryRenderTexture = Helper::makeFactoryPoolWithListener<RenderTexture, 128>( this, &RenderTextureService::onRenderTextureDestroy_, MENGINE_DOCUMENT_FACTORABLE );

        m_factoryDecoderRenderImageProvider = Helper::makeFactoryPool<DecoderRenderImageProvider, 128>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryDecoderRenderImageLoader = Helper::makeFactoryPool<DecoderRenderImageLoader, 128>( MENGINE_DOCUMENT_FACTORABLE );

        uint32_t TextureHashTableSize = CONFIG_VALUE( "Engine", "TextureHashTableSize", 1024 * 8 );

        m_textures.reserve( TextureHashTableSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureService::_finalizeService()
    {
        for( MapRenderTextureEntry::const_iterator
            it = m_textures.begin(),
            it_end = m_textures.end();
            it != it_end;
            ++it )
        {
            const MapRenderTextureEntry::value_type & value = *it;

            RenderTextureInterface * texture = value.element;

            texture->release();
        }

        m_textures.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryRenderTexture );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDecoderRenderImageProvider );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDecoderRenderImageLoader );

        m_factoryRenderTexture = nullptr;
        m_factoryDecoderRenderImageProvider = nullptr;
        m_factoryDecoderRenderImageLoader = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureService::hasTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, RenderTextureInterfacePtr * const _texture ) const
    {
        const ConstString & fileGroupName = _fileGroup->getName();

        const RenderTextureInterface * texture = m_textures.find( fileGroupName, _filePath );

        if( texture == nullptr )
        {
            if( _texture != nullptr )
            {
                *_texture = nullptr;
            }

            return false;
        }

        if( _texture != nullptr )
        {
            *_texture = texture;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureService::getTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) const
    {
        const ConstString & fileGroupName = _fileGroup->getName();

        const RenderTextureInterface * texture = m_textures.find( fileGroupName, _filePath );

        if( texture == nullptr )
        {
            return nullptr;
        }

        return RenderTextureInterfacePtr( texture );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureService::createTexture( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentPtr & _doc )
    {
        RenderImageInterfacePtr image = RENDER_SYSTEM()
            ->createImage( _mipmaps, _width, _height, _format, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( image, "invalid create image %ux%u"
            , _width
            , _height
        );

        RenderTextureInterfacePtr texture = this->createRenderTexture( image, _width, _height, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( texture, "invalid create render texture %ux%u"
            , _width
            , _height
        );

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureService::saveImage( const RenderTextureInterfacePtr & _texture, const FileGroupInterfacePtr & _fileGroup, const ConstString & _codecType, const FilePath & _filePath )
    {
        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( _fileGroup, _filePath, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't create file '%s' '%s'"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
        );

        ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE()
            ->createEncoderT<ImageEncoderInterfacePtr>( _codecType, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( imageEncoder, "can't create encoder for file '%s'"
            , _filePath.c_str()
        );

        if( imageEncoder->initialize( stream ) == false )
        {
            LOGGER_ERROR( "can't initialize encoder for file '%s'"
                , _filePath.c_str()
            );

            return false;
        }

        ImageCodecDataInfo dataInfo;
        dataInfo.mipmaps = 1;
        dataInfo.width = _texture->getWidth();
        dataInfo.height = _texture->getHeight();

        const RenderImageInterfacePtr & image = _texture->getImage();
        dataInfo.format = image->getHWPixelFormat();        

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = dataInfo.width;
        rect.bottom = dataInfo.height;

        RenderImageLockedInterfacePtr locked = image->lock( 0, rect, true );

        size_t pitch = 0;
        void * buffer = locked->getBuffer( &pitch );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "can't lock texture '%s'"
            , _filePath.c_str()
        );

        size_t bufferSize = pitch * dataInfo.height;

        ImageEncoderData data;
        data.buffer = buffer;
        data.size = bufferSize;
        data.pitch = pitch;        

        size_t bytesWritten = imageEncoder->encode( &data, &dataInfo );

        image->unlock( locked, 0, true );

        if( bytesWritten == 0 )
        {
            LOGGER_ERROR( "Error while encoding image data" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureService::visitTexture( const LambdaRenderTexture & _lambda ) const
    {
        for( MapRenderTextureEntry::const_iterator
            it = m_textures.begin(),
            it_end = m_textures.end();
            it != it_end;
            ++it )
        {
            const MapRenderTextureEntry::value_type & value = *it;

            const RenderTextureInterface * texture = value.element;

            _lambda( RenderTextureInterfacePtr( texture ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageLoaderInterfacePtr RenderTextureService::createDecoderRenderImageLoader( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, uint32_t _codecFlags, const DocumentPtr & _doc )
    {
        DecoderRenderImageLoaderPtr loader = Helper::makeFactorableUnique<DecoderRenderImageLoader>( _doc );

        if( loader->initialize( _fileGroup, _filePath, _codecType, _codecFlags ) == false )
        {
            return nullptr;
        }

        return loader;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureService::cacheFileTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const RenderTextureInterfacePtr & _texture )
    {
        _texture->setFileGroup( _fileGroup );
        _texture->setFilePath( _filePath );

        RenderTextureInterface * texture_ptr = _texture.get();

        const ConstString & fileGroupName = _fileGroup->getName();

        m_textures.emplace( fileGroupName, _filePath, texture_ptr );

        LOGGER_INFO( "texture", "cache texture '%s:%s'"
            , fileGroupName.c_str()
            , _filePath.c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureService::loadTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, uint32_t _codecFlags, const DocumentPtr & _doc )
    {
        const ConstString & fileGroupName = _fileGroup->getName();

        const RenderTextureInterface * texture = m_textures.find( fileGroupName, _filePath );

        if( texture != nullptr )
        {
            return RenderTextureInterfacePtr::from( texture );
        }

        if( SERVICE_EXIST( GraveyardServiceInterface ) == true )
        {
            RenderTextureInterfacePtr resurrect_texture = GRAVEYARD_SERVICE()
                ->resurrectTexture( _fileGroup, _filePath, _doc );

            if( resurrect_texture != nullptr )
            {
                this->cacheFileTexture( _fileGroup, _filePath, resurrect_texture );

                return resurrect_texture;
            }
        }

        LOGGER_INFO( "texture", "load texture '%s:%s' codec '%s'"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
            , _codecType.c_str()
        );

        DecoderRenderImageProviderPtr imageProvider = m_factoryDecoderRenderImageProvider->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( imageProvider, "invalid create render image provider" );

        imageProvider->initialize( _fileGroup, _filePath, _codecType, _codecFlags );

        RenderImageLoaderInterfacePtr imageLoader = imageProvider->getLoader( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( imageLoader, "invalid get image loader" );

        RenderImageDesc imageDesc;
        imageLoader->getImageDesc( &imageDesc );

        RenderTextureInterfacePtr new_texture = this->createTexture( imageDesc.mipmaps, imageDesc.width, imageDesc.height, imageDesc.format, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( new_texture, "create texture '%s:%s' codec '%s'"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
            , _codecType.c_str()
        );

        const RenderImageInterfacePtr & image = new_texture->getImage();

        MENGINE_ASSERTION_MEMORY_PANIC( image, "invalid get image" );

        if( imageLoader->load( image ) == false )
        {
            LOGGER_ERROR( "invalid decode image '%s' codec '%s'"
                , _filePath.c_str()
                , _codecType.c_str()
            );

            return nullptr;
        }

        image->setRenderImageProvider( imageProvider );

        this->cacheFileTexture( _fileGroup, _filePath, new_texture );

        return new_texture;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureService::onRenderTextureDestroy_( RenderTextureInterface * _texture )
    {
        const FileGroupInterfacePtr & fileGroup = _texture->getFileGroup();
        const FilePath & filePath = _texture->getFilePath();

        if( filePath.empty() == false )
        {
            const ConstString & fileGroupName = fileGroup->getName();

            RenderTextureInterface * erase_texture = m_textures.erase( fileGroupName, filePath );
            MENGINE_UNUSED( erase_texture );

            MENGINE_ASSERTION_MEMORY_PANIC( erase_texture );

            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_TEXTURE_DESTROY, _texture );
        }

        _texture->release();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureService::createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height, const DocumentPtr & _doc )
    {
        UniqueId id = GENERATE_UNIQUE_IDENTITY();

        RenderTexturePtr texture = m_factoryRenderTexture->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( texture );

        texture->initialize( id, _image, _width, _height );

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
}