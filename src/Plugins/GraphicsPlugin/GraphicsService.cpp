#include "GraphicsService.h"

#include "Interface/ConfigServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "GraphicsGradient.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GraphicsService, Mengine::GraphicsService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void * gp_malloc( gp_size_t _size, void * _ud )
        {
            MENGINE_UNUSED( _ud );

            void * p = Helper::allocateMemory( _size, "gp" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * gp_realloc( void * _ptr, gp_size_t _size, void * _ud )
        {
            MENGINE_UNUSED( _ud );

            void * p = Helper::reallocateMemory( _ptr, _size, "gp" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void gp_free( void * _ptr, void * _ud )
        {
            MENGINE_UNUSED( _ud );

            Helper::deallocateMemory( _ptr, "gp" );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsService::GraphicsService()
        : m_graphics( nullptr )
        , m_linearGradientStage( nullptr )
        , m_radialGradientStage( nullptr )
        , m_gradientTextureGeneration( 1 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsService::~GraphicsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & GraphicsService::requiredServices() const
    {
        static const ServiceRequiredList required = {
            SERVICE_ID( ConfigServiceInterface ),
            SERVICE_ID( NotificationServiceInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GraphicsService::_initializeService()
    {
        gp_size_t GraphicsPlugin_CanvasRecordingMemorySize = CONFIG_VALUE_INTEGER( "GraphicsPlugin", "CanvasRecordingMemorySize", (gp_size_t)GP_CANVAS_RECORDING_MEMORY_SIZE );
        gp_size_t GraphicsPlugin_CanvasRenderOrderCapacity = CONFIG_VALUE_INTEGER( "GraphicsPlugin", "CanvasRenderOrderCapacity", (gp_size_t)GP_CANVAS_RENDER_ORDER_CAPACITY );
        gp_size_t GraphicsPlugin_CanvasScratchMemorySize = CONFIG_VALUE_INTEGER( "GraphicsPlugin", "CanvasScratchMemorySize", (gp_size_t)GP_CANVAS_SCRATCH_MEMORY_SIZE );

        gp_graphics_t * graphics;
        if( gp_graphics_create( &graphics, &Detail::gp_malloc, &Detail::gp_realloc, &Detail::gp_free, nullptr
            , GraphicsPlugin_CanvasRecordingMemorySize
            , GraphicsPlugin_CanvasRenderOrderCapacity
            , GraphicsPlugin_CanvasScratchMemorySize ) == GP_FAILURE )
        {
            LOGGER_ERROR( "invalid create graphics" );

            return false;
        }

        m_graphics = graphics;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE, &GraphicsService::notifyRenderDeviceLostRestore_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE );

        gp_graphics_destroy( m_graphics );
        m_graphics = nullptr;
        m_linearGradientStage = nullptr;
        m_radialGradientStage = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    gp_graphics_t * GraphicsService::getGraphics() const
    {
        return m_graphics;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialStage * GraphicsService::getGradientMaterialStage( bool _radial )
    {
        const RenderMaterialStage * & stage = _radial == true ? m_radialGradientStage : m_linearGradientStage;

        if( stage == nullptr )
        {
            stage = Helper::createGraphicsGradientStage( _radial, MENGINE_DOCUMENT_FACTORABLE );
        }

        return stage;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t GraphicsService::getGradientTextureGeneration() const
    {
        return m_gradientTextureGeneration;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsService::notifyRenderDeviceLostRestore_()
    {
        ++m_gradientTextureGeneration;
    }
    //////////////////////////////////////////////////////////////////////////
}
