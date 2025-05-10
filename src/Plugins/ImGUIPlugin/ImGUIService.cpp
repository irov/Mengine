#include "ImGUIService.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
#include "Environment/Windows/Win32PlatformServiceExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
#include "Environment/SDL2/SDL2PlatformServiceExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
#include "Environment/DirectX9/DX9RenderSystemExtensionInterface.h"
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
#include "Kernel/PrototypeHelper.h"

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

#ifndef MENGINE_IMGUI_INI_FILE
#define MENGINE_IMGUI_INI_FILE "imgui.ini"
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

        if( userFileGroup->existFile( STRINGIZE_FILEPATH_LOCAL_I( MENGINE_IMGUI_INI_FILE ), false ) == true )
        {
            MemoryInterfacePtr memory = Helper::createMemoryFile( userFileGroup, STRINGIZE_FILEPATH_LOCAL_I( MENGINE_IMGUI_INI_FILE ), false, false, MENGINE_DOCUMENT_FACTORABLE );

            const Char * buffer = memory->getBuffer();
            size_t size = memory->getSize();

            if( size != 0 )
            {
                ImGui::LoadIniSettingsFromMemory( buffer, size );
            }
        }

        m_renderProvider = Helper::makeFactorableUnique<ImGUIRenderProvider>( MENGINE_DOCUMENT_FACTORABLE );

        ImGUIRenderPrototypeGeneratorPtr imGUIRenderPrototypeGenerator = Helper::makeFactorableUnique<ImGUIRenderPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE );

        imGUIRenderPrototypeGenerator->setImGUIRenderProvider( m_renderProvider );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( Node::getFactorableType(), ImGUIRender::getFactorableType(), imGUIRenderPrototypeGenerator ) == false )
        {
            return false;
        }
        
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_ATACH_WINDOW, &ImGUIService::notifyPlatformAtachWindow_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_DETACH_WINDOW, &ImGUIService::notifyPlatformDetachWindow_, MENGINE_DOCUMENT_FACTORABLE );
        
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_CREATE, &ImGUIService::notifyRenderDeviceCreate_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_DESTROY, &ImGUIService::notifyRenderDeviceDestroy_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE, &ImGUIService::notifyRenderDeviceLostPrepare_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE, &ImGUIService::notifyRenderDeviceLostRestore_, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        SDL2PlatformServiceExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
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
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_CREATE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_DESTROY );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE );

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            Win32PlatformServiceExtensionInterface * win32Platform = PLATFORM_SERVICE()
                ->getDynamicUnknown();

            win32Platform->removeWin32ProcessHandler( m_handlerId );
            m_handlerId = INVALID_UNIQUE_ID;
        }
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            SDL2PlatformServiceExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
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

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        ImGui_ImplSDL2_Shutdown();
#endif

        ImGui::DestroyContext();

        Helper::removeNodePrototype<ImGUIRender>();

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
        Win32PlatformServiceExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        HWND hWnd = win32Platform->getWindowHandle();

        ImGui_ImplWin32_Init( hWnd );

        UniqueId handlerId = win32Platform->addWin32ProcessHandler( []( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL * const pHandled )
        {
            if( ImGui::IsWindowFocused( ImGuiFocusedFlags_AnyWindow ) == true )
            {
                switch( msg )
                {
                case WM_MOUSEMOVE:
                case WM_NCMOUSEMOVE:
                case WM_MOUSELEAVE:
                case WM_NCMOUSELEAVE:
                case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
                case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
                case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
                case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
                case WM_LBUTTONUP:
                case WM_RBUTTONUP:
                case WM_MBUTTONUP:
                case WM_XBUTTONUP:
                case WM_MOUSEWHEEL:
                case WM_MOUSEHWHEEL:
                case WM_KEYDOWN:
                case WM_KEYUP:
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                case WM_CHAR:
                case WM_SETCURSOR:
                    *pHandled = TRUE;
                    break;
                default:
                    *pHandled = FALSE;
                    break;
                }
            }
            else
            {
                *pHandled = FALSE;
            }

            LRESULT result = ImGui_ImplWin32_WndProcHandler( hwnd, msg, wParam, lParam );

            return result;
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_handlerId = handlerId;
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        SDL2PlatformServiceExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        SDL_Window * window = sdlPlatform->getWindow();
        SDL_GLContext gl_context = sdlPlatform->getGLContext();

        ImGui_ImplSDL2_InitForOpenGL( window, gl_context );        

        UniqueId handlerId = sdlPlatform->addSDLEventHandler( []( SDL_Event * _event )
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
            Win32PlatformServiceExtensionInterface * win32Platform = PLATFORM_SERVICE()
                ->getDynamicUnknown();

            win32Platform->removeWin32ProcessHandler( m_handlerId );
            m_handlerId = INVALID_UNIQUE_ID;
        }

        ImGui_ImplWin32_Shutdown();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            SDL2PlatformServiceExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
                ->getDynamicUnknown();

            sdlPlatform->removeSDLEventHandler( m_handlerId );
            m_handlerId = INVALID_UNIQUE_ID;
        }

        ImGui_ImplSDL2_Shutdown();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIService::notifyRenderDeviceCreate_()
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
    void ImGUIService::notifyRenderDeviceDestroy_()
    {
#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        ImGui_ImplDX9_Shutdown();
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        ImGui_ImplOpenGL2_DestroyFontsTexture();
        ImGui_ImplOpenGL2_DestroyDeviceObjects();

        ImGui_ImplOpenGL2_Shutdown();
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