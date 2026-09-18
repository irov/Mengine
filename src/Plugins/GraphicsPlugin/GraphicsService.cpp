#include "GraphicsService.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryAllocator.h"

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
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsService::~GraphicsService()
    {
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsService::_finalizeService()
    {
        gp_graphics_destroy( m_graphics );
        m_graphics = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    gp_graphics_t * GraphicsService::getGraphics() const
    {
        return m_graphics;
    }
    //////////////////////////////////////////////////////////////////////////
}
