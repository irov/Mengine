#include "MosaicService.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Plugins/GraphicsPlugin/GraphicsInterface.h"

#include "MosaicRender.h"
#include "MosaicRenderPrototypeGenerator.h"
#include "MosaicSerializer.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/Assertion.h"

#include "Config/StdAlgorithm.h"

#include "Mosaic/Mosaic.hpp"

#ifndef MENGINE_MOSAIC_SETTINGS_FILE
#define MENGINE_MOSAIC_SETTINGS_FILE "mosaic.settings"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( MosaicService, Mengine::MosaicService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MosaicService::MosaicService()
        : m_context( nullptr )
        , m_fontProvider( nullptr )
        , m_providerEnumerator( MOSAIC_INVALID_PROVIDER_ID )
        , m_handlerId( INVALID_UNIQUE_ID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicService::~MosaicService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicService::_dependencyService()
    {
        SERVICE_DEPENDENCY( MosaicService, GraphicsServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicService::_initializeService()
    {
        Mosaic::setDefaultAllocator( &m_allocator );

        m_fontProvider = Helper::allocateMemoryT<MosaicFontProvider>( "mosaic" );

        if( m_fontProvider == nullptr )
        {
            return false;
        }

        new (m_fontProvider) MosaicFontProvider( &m_renderer );

        Mosaic::ContextOptions options;
        options.allocator = &m_allocator;
        options.platform = &m_platform;
        options.fontProvider = m_fontProvider;

        m_context = Mosaic::newContext( options );

        if( m_context == nullptr )
        {
            LOGGER_ERROR( "invalid create mosaic context" );

            return false;
        }

        Mosaic::Configuration configuration;

        if( Mosaic::getConfiguration( m_context, &configuration ) == true )
        {
            configuration.dragClickToInputText = true;

            Mosaic::setConfiguration( m_context, configuration );
        }

        this->loadSettings_();

        MosaicRenderPrototypeGeneratorPtr generator = Helper::makeFactorableUnique<MosaicRenderPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MosaicRender" ), generator ) == false )
        {
            return false;
        }

        SERVICE_WAIT_METHOD( GraphicsServiceInterface, this, initializeGraphicsBridge_ );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_DESTROY, &MosaicService::notifyRenderDeviceDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE, &MosaicService::notifyRenderDeviceLostPrepare_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE, &MosaicService::notifyRenderDeviceLostRestore_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_DESTROY );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE );

        m_providers.clear();

        if( m_context != nullptr )
        {
            this->saveSettings_();

            Mosaic::deleteContext( m_context );
            m_context = nullptr;
        }

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MosaicRender" ), nullptr );

        if( m_fontProvider != nullptr )
        {
            m_fontProvider->finalize();

            m_fontProvider->~MosaicFontProvider();

            Helper::deallocateMemory( m_fontProvider, "mosaic" );
            m_fontProvider = nullptr;
        }

        m_renderer.finalize();

        m_bridge.finalize();

        Mosaic::setDefaultAllocator( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicService::initializeGraphicsBridge_()
    {
        gp_graphics_t * graphics = GRAPHICS_SERVICE()
            ->getGraphics();

        if( m_bridge.initialize( graphics, &m_allocator ) == false )
        {
            Mosaic::StringView error = m_bridge.lastError();

            LOGGER_ERROR( "invalid initialize mosaic graphics bridge: %.*s"
                , (int32_t)error.size()
                , error.data()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const Mosaic::RenderMesh * MosaicService::prepareRenderMesh( const Mosaic::Frame & _frame )
    {
        if( m_bridge.prepare( _frame ) == false )
        {
            Mosaic::StringView error = m_bridge.lastError();

            if( error.empty() == false )
            {
                LOGGER_ERROR( "mosaic graphics bridge: %.*s"
                    , (int32_t)error.size()
                    , error.data()
                );
            }

            return nullptr;
        }

        const Mosaic::RenderMesh * mesh = m_bridge.renderData();

        return mesh;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicService::_stopService()
    {
        if( m_fontProvider != nullptr )
        {
            m_fontProvider->finalize();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::Context * MosaicService::getContext() const
    {
        return m_context;
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicTextureHandle MosaicService::getTextureHandle( const RenderTextureInterfacePtr & _texture )
    {
        Mosaic::TextureHandle handle = m_renderer.acquireTexture( _texture );

        return (MosaicTextureHandle)handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicService::releaseTextureHandle( MosaicTextureHandle _handle )
    {
        m_renderer.releaseTexture( (Mosaic::TextureHandle)_handle );
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicProviderId MosaicService::addProvider( const UnknownMosaicRenderInterface::LambdaMosaicProvider & _provider )
    {
        MosaicProviderId id = ++m_providerEnumerator;

        MosaicProviderDesc desc;
        desc.id = id;
        desc.provider = _provider;

        m_providers.emplace_back( desc );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicService::removeProvider( MosaicProviderId _id )
    {
        VectorMosaicProviders::const_iterator it_found = StdAlgorithm::find_if( m_providers.begin(), m_providers.end(), [_id]( const MosaicProviderDesc & _desc )
        {
            return _desc.id == _id;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_providers.end(), "mosaic remove unknown provider '%u'"
            , _id
        );

        m_providers.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorMosaicProviders & MosaicService::getProviders() const
    {
        return m_providers;
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicRendererAdapter * MosaicService::getRendererAdapter()
    {
        return &m_renderer;
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicFontProvider * MosaicService::getFontProvider()
    {
        return m_fontProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicService::notifyRenderDeviceDestroy_()
    {
        m_renderer.finalize();

        if( m_fontProvider != nullptr )
        {
            m_fontProvider->finalize();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicService::notifyRenderDeviceLostPrepare_()
    {
        m_renderer.finalize();

        if( m_fontProvider != nullptr )
        {
            m_fontProvider->finalize();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicService::notifyRenderDeviceLostRestore_()
    {
        if( m_fontProvider != nullptr )
        {
            m_fontProvider->invalidate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicService::loadSettings_()
    {
        const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        if( userFileGroup == nullptr )
        {
            return;
        }

        if( userFileGroup->existFile( STRINGIZE_FILEPATH_LOCAL_I( MENGINE_MOSAIC_SETTINGS_FILE ), false ) == false )
        {
            return;
        }

        MemoryInterfacePtr memory = Helper::createMemoryFile( userFileGroup, STRINGIZE_FILEPATH_LOCAL_I( MENGINE_MOSAIC_SETTINGS_FILE ), false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( memory == nullptr )
        {
            return;
        }

        const Char * buffer = memory->getBuffer();
        size_t size = memory->getSize();

        if( size == 0 )
        {
            return;
        }

        MosaicDeserializer deserializer;

        if( deserializer.parse( buffer, size ) == false )
        {
            return;
        }

        Mosaic::deserialize( m_context, deserializer );
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicService::saveSettings_()
    {
        const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        if( userFileGroup == nullptr )
        {
            return;
        }

        MosaicSerializer serializer;

        if( Mosaic::serialize( m_context, serializer ) == false )
        {
            return;
        }

        const String & buffer = serializer.getBuffer();

        if( buffer.empty() == true )
        {
            if( userFileGroup->existFile( STRINGIZE_FILEPATH_LOCAL_I( MENGINE_MOSAIC_SETTINGS_FILE ), false ) == true )
            {
                userFileGroup->removeFile( STRINGIZE_FILEPATH_LOCAL_I( MENGINE_MOSAIC_SETTINGS_FILE ) );
            }

            return;
        }

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( userFileGroup, STRINGIZE_FILEPATH_LOCAL_I( MENGINE_MOSAIC_SETTINGS_FILE ), true, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            return;
        }

        size_t buffer_size = buffer.size();

        bool successful = stream->write( buffer.c_str(), buffer_size ) == buffer_size;

        Helper::closeOutputStreamFile( userFileGroup, stream, successful );
    }
    //////////////////////////////////////////////////////////////////////////
}
