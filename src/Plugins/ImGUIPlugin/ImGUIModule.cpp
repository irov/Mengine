#include "ImGUIModule.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PlatformInterface.h"

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
#include "Interface/Win32PlatformExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
#include "Interface/SDLPlatformExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
#include "Interface/DX9RenderSystemExtensionInterface.h"
#endif

#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderServiceInterface.h"

#include "ImGUIRender.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/FileStreamHelper.h"

#include "imgui.h"

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
#include "imgui_impl_win32.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
#include "imgui_impl_sdl.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
#include "imgui_impl_dx9.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
#include "imgui_impl_opengl2.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
#endif

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void * ImGUIPlugin_alloc_func( size_t sz, void * user_data )
        {
            MENGINE_UNUSED( user_data );

            void * p = Helper::allocateMemory( sz, "imgui" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////    
        static void ImGUIPlugin_free_func( void * ptr, void * user_data )
        {
            MENGINE_UNUSED( user_data );

            Helper::deallocateMemory( ptr, "imgui" );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ImGUIModule::ImGUIModule()
        : m_handlerId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImGUIModule::~ImGUIModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImGUIModule::_initializeModule()
    {
        LOGGER_INFO( "imgui", "Initializing ImGUI Module ..." );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ImGUIRender" ), Helper::makeFactorableUnique<NodePrototypeGenerator<ImGUIRender, 16>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        ImGui::SetAllocatorFunctions( &Detail::ImGUIPlugin_alloc_func, &Detail::ImGUIPlugin_free_func, nullptr );

        ImGui::CreateContext();

        ImGuiIO & io = ImGui::GetIO();

        io.LogFilename = nullptr;
        io.IniFilename = nullptr;

        const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        if( userFileGroup->existFile( STRINGIZE_FILEPATH_LOCAL( "imgui.ini" ), false ) == true )
        {
            MemoryInterfacePtr memory = Helper::createMemoryFile( userFileGroup, STRINGIZE_FILEPATH_LOCAL( "imgui.ini" ), false, false, MENGINE_DOCUMENT_FACTORABLE );

            const Char * buffer = memory->getBuffer();
            size_t size = memory->getSize();

            ImGui::LoadIniSettingsFromMemory( buffer, size );
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CREATE_RENDER_WINDOW, &ImGUIModule::notifyCreateRenderWindow_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE, &ImGUIModule::notifyRenderDeviceLostPrepare_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE, &ImGUIModule::notifyRenderDeviceLostRestore_, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getUnknown();

        uint32_t handlerId = win32Platform->addWin32ProcessHandler( []( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL * const pHandled )
        {
            *pHandled = FALSE;

            LRESULT result = ImGui_ImplWin32_WndProcHandler( hwnd, msg, wParam, lParam );

            return result;
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_handlerId = handlerId;
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        SDLPlatformExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
            ->getUnknown();

        uint32_t handlerId = sdlPlatform->addSDLEventHandler( []( SDL_Event * _event )
        {
            ImGui_ImplSDL2_ProcessEvent( _event );
        } );

        m_handlerId = handlerId;
#endif
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIModule::_finalizeModule()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CREATE_RENDER_WINDOW );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE );

        const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( userFileGroup, STRINGIZE_FILEPATH_LOCAL( "imgui.ini" ), MENGINE_DOCUMENT_FACTORABLE );

        size_t imIniSettingsSize;
        const char * imIniSettings = ImGui::SaveIniSettingsToMemory( &imIniSettingsSize );

        stream->write( imIniSettings, imIniSettingsSize );
        stream->flush();

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
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
            ->getUnknown();

        win32Platform->removeWin32ProcessHandler( m_handlerId );
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        SDLPlatformExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
            ->getUnknown();

        sdlPlatform->removeSDLEventHandler( m_handlerId );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIModule::notifyCreateRenderWindow_()
    {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL) && defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        SDLPlatformExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
            ->getUnknown();

        SDL_Window * window = sdlPlatform->getWindow();
        SDL_GLContext gl_context = sdlPlatform->getGLContext();

        ImGui_ImplSDL2_InitForOpenGL( window, gl_context );

        ImGui_ImplOpenGL2_Init();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32) && defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getUnknown();

        HWND hWnd = win32Platform->getWindowHandle();

        ImGui_ImplWin32_Init( hWnd );

        DX9RenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        if( extension != nullptr )
        {
            IDirect3DDevice9 * d3dDevice = extension->getDirect3DDevice9();

            ImGui_ImplDX9_Init( d3dDevice );
            ImGui_ImplDX9_CreateDeviceObjects();
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIModule::notifyRenderDeviceLostPrepare_()
    {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32) && defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        ImGui_ImplDX9_InvalidateDeviceObjects();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIModule::notifyRenderDeviceLostRestore_()
    {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32) && defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        ImGui_ImplDX9_CreateDeviceObjects();
#endif
    }
}