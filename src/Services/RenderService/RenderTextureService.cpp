#include "RenderTextureService.h"

#include "Interface/WatchdogServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/GraveyardServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "RenderTexture.h"
#include "DecoderRenderImageProvider.h"
#include "DecoderRenderImageLoader.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/NotificationHelper.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderTextureService, Mengine::RenderTextureService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderTextureService::RenderTextureService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureService::~RenderTextureService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureService::_initializeService()
    {
        m_factoryRenderTexture = Helper::makeFactoryPoolWithListener<RenderTexture, 128>( this, &RenderTextureService::onRenderTextureDestroy_, MENGINE_DOCUMENT_FACTORABLE );

        m_factoryDecoderRenderImageProvider = Helper::makeFactoryPool<DecoderRenderImageProvider, 128>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryDecoderRenderImageLoader = Helper::makeFactoryPool<DecoderRenderImageLoader, 128>( MENGINE_DOCUMENT_FACTORABLE );

        uint32_t Engine_TextureHashTableSize = CONFIG_VALUE( "Engine", "TextureHashTableSize", 1024 * 8 );

        m_textures.reserve( Engine_TextureHashTableSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureService::_finalizeService()
    {
        for( const MapRenderTextureEntry::value_type & value : m_textures )
        {
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
    bool RenderTextureService::hasTexture( const ContentInterfacePtr & _content, RenderTextureInterfacePtr * const _texture ) const
    {
        const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();
        const FilePath & filePath = _content->getFilePath();

        const RenderTextureInterface * texture = m_textures.find( fileGroup, filePath );

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
    RenderTextureInterfacePtr RenderTextureService::getTexture( const ContentInterfacePtr & _content ) const
    {
        const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();
        const FilePath & filePath = _content->getFilePath();

        const RenderTextureInterface * texture = m_textures.find( fileGroup, filePath );

        if( texture == nullptr )
        {
            return nullptr;
        }

        return RenderTextureInterfacePtr( texture );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureService::createTexture( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
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
    bool RenderTextureService::saveImage( const RenderTextureInterfacePtr & _texture, const ContentInterfacePtr & _content )
    {
        OutputStreamInterfacePtr stream = _content->openOutputStreamFile( true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't create file '%s'"
            , Helper::getContentFullPath( _content )
        );

        const ConstString & codecType = _content->getCodecType();

        ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE()
            ->createEncoder( codecType, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( imageEncoder, "can't create encoder for file '%s'"
            , Helper::getContentFullPath( _content )
        );

        if( imageEncoder->initialize( stream ) == false )
        {
            LOGGER_ERROR( "can't initialize encoder for file '%s'"
                , Helper::getContentFullPath( _content )
            );

            return false;
        }

        const RenderImageInterfacePtr & image = _texture->getImage();

        uint32_t hwWidth = image->getHWWidth();
        uint32_t hwHeight = image->getHWHeight();
        EPixelFormat hwPixelFormat = image->getHWPixelFormat();

        ImageCodecDataInfo dataInfo;
        dataInfo.mipmaps = 1;
        dataInfo.width = hwWidth;
        dataInfo.height = hwHeight;
        dataInfo.format = hwPixelFormat;

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = dataInfo.width;
        rect.bottom = dataInfo.height;

        RenderImageLockedInterfacePtr locked = image->lock( 0, rect, true );

        size_t pitch = 0;
        void * buffer = locked->getBuffer( &pitch );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "can't lock texture '%s'"
            , Helper::getContentFullPath( _content )
        );

        size_t bufferSize = pitch * dataInfo.height;

        ImageEncoderData data;
        data.buffer = buffer;
        data.size = bufferSize;
        data.pitch = pitch;        

        size_t bytesWritten = imageEncoder->encode( &data, &dataInfo );

        imageEncoder->finalize();

        image->unlock( locked, 0, true );

        if( _content->closeOutputStreamFile( stream ) == false )
        {
            LOGGER_ERROR( "can't close file '%s'"
                , Helper::getContentFullPath( _content )
            );

            return false;
        }

        if( bytesWritten == 0 )
        {
            LOGGER_ERROR( "Error while encoding image data" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureService::foreachTexture( const LambdaRenderTexture & _lambda ) const
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
    RenderImageLoaderInterfacePtr RenderTextureService::createDecoderRenderImageLoader( const ContentInterfacePtr & _content, uint32_t _codecFlags, const DocumentInterfacePtr & _doc )
    {
        DecoderRenderImageLoaderPtr loader = Helper::makeFactorableUnique<DecoderRenderImageLoader>( _doc );

        if( loader->initialize( _content, _codecFlags ) == false )
        {
            return nullptr;
        }

        return loader;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTextureService::cacheFileTexture( const ContentInterfacePtr & _content, const RenderTextureInterfacePtr & _texture )
    {
        _texture->setContent( _content );

        const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();
        const FilePath & filePath = _content->getFilePath();

        RenderTextureInterface * texture_ptr = _texture.get();

        m_textures.emplace( fileGroup, filePath, texture_ptr );

        LOGGER_INFO( "texture", "cache texture '%s'"
            , Helper::getContentFullPath( _content )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureService::loadTexture( const ContentInterfacePtr & _content, uint32_t _codecFlags, uint32_t _width, uint32_t _height, const DocumentInterfacePtr & _doc )
    {
        const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();
        const FilePath & filePath = _content->getFilePath();

        const RenderTextureInterface * texture = m_textures.find( fileGroup, filePath );

        if( texture != nullptr )
        {
            return RenderTextureInterfacePtr::from( texture );
        }

        if( SERVICE_IS_INITIALIZE( GraveyardServiceInterface ) == true )
        {
            RenderTextureInterfacePtr resurrect_texture = GRAVEYARD_SERVICE()
                ->resurrectTexture( _content, _doc );

            if( resurrect_texture != nullptr )
            {
                this->cacheFileTexture( _content, resurrect_texture );

                return resurrect_texture;
            }
        }

        LOGGER_INFO( "texture", "load texture '%s' codec '%s'"
            , Helper::getContentFullPath( _content )
            , _content->getCodecType().c_str()
        );

        DecoderRenderImageProviderPtr imageProvider = m_factoryDecoderRenderImageProvider->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( imageProvider, "invalid create render image provider" );

        imageProvider->initialize( _content, _codecFlags );

        RenderImageLoaderInterfacePtr imageLoader = imageProvider->getLoader( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( imageLoader, "invalid get image loader" );

        RenderImageDesc imageDesc;
        imageLoader->getImageDesc( &imageDesc );

        if( _width != ~0U )
        {
            imageDesc.width = _width;
        }

        if( _height != ~0U )
        {
            imageDesc.height = _height;
        }

        RenderTextureInterfacePtr new_texture = this->createTexture( imageDesc.mipmaps, imageDesc.width, imageDesc.height, imageDesc.format, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( new_texture, "create texture '%s' codec '%s'"
            , Helper::getContentFullPath( _content )
            , _content->getCodecType().c_str()
        );

        const RenderImageInterfacePtr & image = new_texture->getImage();

        MENGINE_ASSERTION_MEMORY_PANIC( image, "invalid get image" );

        if( imageLoader->load( image ) == false )
        {
            LOGGER_ERROR( "invalid decode image '%s' codec '%s'"
                , Helper::getContentFullPath( _content )
                , _content->getCodecType().c_str()
            );

            return nullptr;
        }

        image->setRenderImageProvider( imageProvider );

        this->cacheFileTexture( _content, new_texture );

        return new_texture;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTextureService::onRenderTextureDestroy_( RenderTextureInterface * _texture )
    {
        const ContentInterfacePtr & content = _texture->getContent();

        if( content != nullptr )
        {
            const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
            const FilePath & filePath = content->getFilePath();

            if( filePath.empty() == false )
            {
                RenderTextureInterface * erase_texture = m_textures.erase( fileGroup, filePath );
                MENGINE_UNUSED( erase_texture );

                MENGINE_ASSERTION_MEMORY_PANIC( erase_texture );

                NOTIFICATION_NOTIFY( NOTIFICATOR_RENDER_TEXTURE_DESTROY, _texture );
            }
        }

        _texture->release();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr RenderTextureService::createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height, const DocumentInterfacePtr & _doc )
    {
        UniqueId id = Helper::generateUniqueIdentity();

        RenderTexturePtr texture = m_factoryRenderTexture->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( texture );

        texture->initialize( id, _image, _width, _height );

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
}