#include "ImGUIPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PlatformInterface.h"

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
#include "Interface/Win32PlatformExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
#include "Interface/SDLPlatformExtensionInterface.h"
#endif

#include "Interface/RenderSystemInterface.h"
#include "Interface/AllocatorServiceInterface.h"
#include "Interface/RenderServiceInterface.h"

#include "ImGUIRender.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/DefaultPrototypeGenerator.h"

#include "imgui.h"

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
#include "imgui_impl_win32.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
#include "imgui_impl_sdl.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX)
#include "imgui_impl_dx9.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
#include "imgui_impl_opengl2.h"
#endif

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
#endif
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ImGUI, Mengine::ImGUIPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void * ImGUIPlugin_alloc_func( size_t sz, void * user_data )
    {
        MENGINE_UNUSED( user_data );

        void * p = ALLOCATOR_SERVICE()
            ->malloc( sz, "imgui" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////    
    static void ImGUIPlugin_free_func( void * ptr, void * user_data )
    {
        MENGINE_UNUSED( user_data );

        ALLOCATOR_SERVICE()
            ->free( ptr, "imgui" );
    }
    //////////////////////////////////////////////////////////////////////////
    ImGUIPlugin::ImGUIPlugin()
        : m_handlerId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImGUIPlugin::~ImGUIPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImGUIPlugin::_initializePlugin()
    {
        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ImGUIRender" ), Helper::makeFactorableUnique<NodePrototypeGenerator<ImGUIRender, 16>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        ImGui::SetAllocatorFunctions( &ImGUIPlugin_alloc_func, &ImGUIPlugin_free_func, nullptr );

        ImGui::CreateContext();

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CREATE_RENDER_WINDOW, this, &ImGUIPlugin::notifyCreateRenderWindow_ );

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getPlatformExtention();

        uint32_t handlerId = win32Platform->addWin32ProcessHandler( []( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
        {
            LRESULT result = ImGui_ImplWin32_WndProcHandler( hwnd, msg, wParam, lParam );

            return result;
        } );

        m_handlerId = handlerId;
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        SDLPlatformExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
            ->getPlatformExtention();

        uint32_t handlerId = sdlPlatform->addSDLEventHandler( []( SDL_Event * _event )
        {
            ImGui_ImplSDL2_ProcessEvent( _event );
        } );

        m_handlerId = handlerId;
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIPlugin::_finalizePlugin()
    {
#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX)
        ImGui_ImplDX9_Shutdown();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        ImGui_ImplWin32_Shutdown();
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        ImGui_ImplOpenGL2_DestroyFontsTexture();
        ImGui_ImplOpenGL2_DestroyDeviceObjects();

        ImGui_ImplOpenGL2_Shutdown();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        ImGui_ImplSDL2_Shutdown();
#endif

        ImGui::DestroyContext();

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ImGUIRender" ) );

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getPlatformExtention();

        win32Platform->removeWin32ProcessHandler( m_handlerId );
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        SDLPlatformExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
            ->getPlatformExtention();

        sdlPlatform->removeSDLEventHandler( m_handlerId );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIPlugin::_destroyPlugin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIPlugin::notifyCreateRenderWindow_()
    {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL) && defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        SDLPlatformExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
            ->getPlatformExtention();

        SDL_Window * window = sdlPlatform->getWindow();
        SDL_GLContext gl_context = sdlPlatform->getGLContext();

        ImGui_ImplSDL2_InitForOpenGL( window, gl_context );
        
        ImGui_ImplOpenGL2_Init();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32) && defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX)
        Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getPlatformExtention();

        HWND hWnd = win32Platform->getWindowHandle();

        ImGui_ImplWin32_Init( hWnd );

        IDirect3DDevice9 * d3dDevice = RENDER_SYSTEM()
            ->getRenderDevice();
        
        ImGui_ImplDX9_Init( d3dDevice );
        ImGui_ImplDX9_CreateDeviceObjects();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
