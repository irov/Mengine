#include "ImGUIService.h"

#include "Interface/PrototypeServiceInterface.h"
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
#include "ImGUIRenderProvider.h"
#include "ImGUIRenderPrototypeGenerator.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/NotificationHelper.h"

#include "imgui.h"

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
#include "imgui_impl_win32.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
#include "imgui_impl_sdl2.h"
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

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ImGUIService, Mengine::ImGUIService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
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
    ImGUIService::ImGUIService()
        : m_handlerId( INVALID_UNIQUE_ID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImGUIService::~ImGUIService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImGUIService::_initializeService()
    {
        ImGui::SetAllocatorFunctions( &Detail::ImGUIPlugin_alloc_func, &Detail::ImGUIPlugin_free_func, nullptr );

        ImGui::CreateContext();

        ImGui::StyleColorsDark();

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

        m_renderProvider = Helper::makeFactorableUnique<ImGUIRenderProvider>( MENGINE_DOCUMENT_FACTORABLE );

        ImGUIRenderPrototypeGeneratorPtr imGUIRenderPrototypeGenerator = Helper::makeFactorableUnique<ImGUIRenderPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE );

        imGUIRenderPrototypeGenerator->setImGUIRenderProvider( m_renderProvider );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ImGUIRender" ), imGUIRenderPrototypeGenerator ) == false )
        {
            return false;
        }
        
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_ATACH_WINDOW, &ImGUIService::notifyPlatformAtachWindow_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_DETACH_WINDOW, &ImGUIService::notifyPlatformDetachWindow_, MENGINE_DOCUMENT_FACTORABLE );
        
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CREATE_RENDER_WINDOW, &ImGUIService::notifyCreateRenderWindow_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE, &ImGUIService::notifyRenderDeviceLostPrepare_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE, &ImGUIService::notifyRenderDeviceLostRestore_, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        SDLPlatformExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        uint32_t handlerId = sdlPlatform->addSDLEventHandler( []( SDL_Event * _event )
        {
            ImGui_ImplSDL2_ProcessEvent( _event );
        } );

        m_handlerId = handlerId;
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_ATACH_WINDOW );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_DETACH_WINDOW );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CREATE_RENDER_WINDOW );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE );

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()
                ->getDynamicUnknown();

            win32Platform->removeWin32ProcessHandler( m_handlerId );
            m_handlerId = INVALID_UNIQUE_ID;
        }
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            SDLPlatformExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
                ->getDynamicUnknown();

            sdlPlatform->removeSDLEventHandler( m_handlerId );
            m_handlerId = INVALID_UNIQUE_ID;
        }
#endif

        const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( userFileGroup, STRINGIZE_FILEPATH_LOCAL( "imgui.ini" ), true, MENGINE_DOCUMENT_FACTORABLE );

        size_t imIniSettingsSize;
        const char * imIniSettings = ImGui::SaveIniSettingsToMemory( &imIniSettingsSize );

        stream->write( imIniSettings, imIniSettingsSize );
        stream->flush();

        Helper::closeOutputStreamFile( userFileGroup, stream );

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
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ImGUIRender" ), nullptr );

        m_renderProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const ImGUIRenderProviderInterfacePtr & ImGUIService::getRenderProvider() const
    {
        return m_renderProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIService::notifyPlatformAtachWindow_()
    {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        HWND hWnd = win32Platform->getWindowHandle();

        ImGui_ImplWin32_Init( hWnd );

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
            ->getDynamicUnknown();

        SDL_Window * window = sdlPlatform->getWindow();
        SDL_GLContext gl_context = sdlPlatform->getGLContext();

        ImGui_ImplSDL2_InitForOpenGL( window, gl_context );        

        uint32_t handlerId = sdlPlatform->addSDLEventHandler( []( SDL_Event * _event )
        {
            ImGui_ImplSDL2_ProcessEvent( _event );
        } );

        m_handlerId = handlerId;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIService::notifyPlatformDetachWindow_()
    {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()
                ->getDynamicUnknown();

            win32Platform->removeWin32ProcessHandler( m_handlerId );
            m_handlerId = INVALID_UNIQUE_ID;
        }

        ImGui_ImplWin32_Shutdown();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            SDLPlatformExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
                ->getDynamicUnknown();

            sdlPlatform->removeSDLEventHandler( m_handlerId );
            m_handlerId = INVALID_UNIQUE_ID;
        }

        ImGui_ImplSDL2_Shutdown();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIService::notifyCreateRenderWindow_()
    {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL) && defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        ImGui_ImplOpenGL2_Init();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32) && defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
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
    void ImGUIService::notifyRenderDeviceLostPrepare_()
    {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32) && defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        ImGui_ImplDX9_InvalidateDeviceObjects();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIService::notifyRenderDeviceLostRestore_()
    {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32) && defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        ImGui_ImplDX9_CreateDeviceObjects();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}