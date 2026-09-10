#include "RenderTexturePrefetchJob.h"

#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderTextureServiceInterface.h"

#include "PrefetchedRenderImageProvider.h"

#include "Kernel/ContentHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/TimeDog.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderTexturePrefetchJob::RenderTexturePrefetchJob()
        : m_codecFlags( 0 )
        , m_state( ERenderTexturePrefetchState::ERTPS_QUEUED )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTexturePrefetchJob::~RenderTexturePrefetchJob()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & RenderTexturePrefetchJob::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t RenderTexturePrefetchJob::getCodecFlags() const
    {
        return m_codecFlags;
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderTexturePrefetchState RenderTexturePrefetchJob::getState() const
    {
        return m_state;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTextureInterfacePtr & RenderTexturePrefetchJob::getTexture() const
    {
        return m_texture;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetchJob::initialize( const ContentInterfacePtr & _content, uint32_t _codecFlags )
    {
        m_content = _content;
        m_codecFlags = _codecFlags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTexturePrefetchJob::isPreparing() const
    {
        if( m_task != nullptr )
        {
            return true;
        }

        bool preparing = m_state == ERenderTexturePrefetchState::ERTPS_READY_FOR_UPLOAD;

        return preparing;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTexturePrefetchJob::start()
    {
        m_task = Helper::makeFactorableUnique<ThreadTaskPrefetchTexture>( MENGINE_DOCUMENT_FACTORABLE );
        m_task->initialize( m_content, m_codecFlags );

        if( PREFETCHER_SERVICE()
            ->addPrefetchTask( m_task ) == false )
        {
            m_task = nullptr;
            m_state = ERenderTexturePrefetchState::ERTPS_FAILED;

            LOGGER_ERROR( "texture prefetch unavailable for '%s'"
                , Helper::getContentFullPath( m_content ).c_str()
            );

            return false;
        }

        m_state = ERenderTexturePrefetchState::ERTPS_DECODING;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetchJob::update()
    {
        if( m_task == nullptr )
        {
            return;
        }

        if( m_task->isComplete() == false )
        {
            return;
        }

        if( m_state == ERenderTexturePrefetchState::ERTPS_CANCELLED )
        {
            m_task = nullptr;

            return;
        }

        bool successful = m_task->isSuccessful();

        if( successful == true )
        {
            m_imageLoader = m_task->getImage();
            m_state = ERenderTexturePrefetchState::ERTPS_READY_FOR_UPLOAD;
        }
        else
        {
            m_state = ERenderTexturePrefetchState::ERTPS_FAILED;

            LOGGER_ERROR( "texture prefetch decode failed '%s'"
                , Helper::getContentFullPath( m_content ).c_str()
            );
        }

        m_task = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetchJob::upload()
    {
        TIMEDOG_SCOPE( "texture_prefetch", "upload '%s'"
            , m_content->getFilePath().c_str()
        );

        RenderImageDesc desc;
        m_imageLoader->getImageDesc( &desc );

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->getTexture( m_content );
        bool successful = false;
        bool reused = texture != nullptr;

        if( reused == true )
        {
            successful = this->isCompatibleTexture_( texture, desc );
        }
        else
        {
            RenderImageInterfacePtr image = RENDER_SYSTEM()
                ->createImage( desc.mipmaps, desc.width, desc.height, 1, desc.format, MENGINE_DOCUMENT_FACTORABLE );

            if( image != nullptr )
            {
                successful = m_imageLoader->load( image );
            }

            if( successful == true )
            {
                texture = RENDERTEXTURE_SERVICE()
                    ->createRenderTexture( image, desc.width, desc.height, m_codecFlags, MENGINE_DOCUMENT_FACTORABLE );

                PrefetchedRenderImageProviderPtr provider = Helper::makeFactorableUnique<PrefetchedRenderImageProvider>( MENGINE_DOCUMENT_FACTORABLE );
                provider->initialize( m_content, m_imageLoader );
                image->setRenderImageProvider( provider );

                RENDERTEXTURE_SERVICE()
                    ->cacheFileTexture( m_content, texture );
            }
        }

        if( successful == false )
        {
            m_state = ERenderTexturePrefetchState::ERTPS_FAILED;

            LOGGER_ERROR( "texture prefetch upload or cache compatibility failed '%s'"
                , Helper::getContentFullPath( m_content ).c_str()
            );
        }
        else
        {
            m_texture = texture;
            m_state = ERenderTexturePrefetchState::ERTPS_RESIDENT;
        }

        m_imageLoader = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetchJob::reject()
    {
        m_state = ERenderTexturePrefetchState::ERTPS_FAILED;

        LOGGER_ERROR( "texture prefetch request rejected: invalid content, incompatible request or stopped queue" );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetchJob::cancel()
    {
        m_state = ERenderTexturePrefetchState::ERTPS_CANCELLED;
        m_imageLoader = nullptr;
        m_texture = nullptr;

        if( m_task != nullptr )
        {
            m_task->cancel();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetchJob::stop()
    {
        this->cancel();

        if( m_task != nullptr )
        {
            m_task->join();
            m_task = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTexturePrefetchJob::isCompatibleTexture_( const RenderTextureInterfacePtr & _texture, const RenderImageDesc & _desc ) const
    {
        if( _texture->getCodecFlags() != m_codecFlags )
        {
            return false;
        }

        const ContentInterfacePtr & content = _texture->getContent();
        const ConstString & codecType = content->getCodecType();
        const ConstString & requestedCodecType = m_content->getCodecType();

        if( codecType != requestedCodecType )
        {
            return false;
        }

        if( _texture->getWidth() != _desc.width )
        {
            return false;
        }

        if( _texture->getHeight() != _desc.height )
        {
            return false;
        }

        const RenderImageInterfacePtr & image = _texture->getImage();

        if( image->getHWMipmaps() != _desc.mipmaps )
        {
            return false;
        }

        EPixelFormat format = image->getHWPixelFormat();

        if( format == PF_X8R8G8B8 && _desc.format == PF_A8R8G8B8 )
        {
            return true;
        }

        bool compatible = format == _desc.format;

        return compatible;
    }
    //////////////////////////////////////////////////////////////////////////
}
