#include "ImGUIService.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
#include "Environment/Windows/Win32PlatformServiceExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
#include "Environment/MacOS/MacOSPlatformServiceExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_IOS)
#include "Environment/iOS/iOSPlatformServiceExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX)
#include "Environment/Unix/UnixPlatformServiceExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
#include "Environment/DirectX9/DX9RenderSystemExtensionInterface.h"
#endif

#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderServiceInterface.h"

#include "ImGUIRender.h"
#include "ImGUIRenderProvider.h"
#include "ImGUIRenderPrototypeGenerator.h"

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
#include "ImGUIWin32Platform.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
#include "ImGUIDX9Render.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL) && (defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS) || defined(MENGINE_ENVIRONMENT_PLATFORM_IOS) || defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX))
#include "ImGUIOpenGL3Render.h"
#elif defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
#include "ImGUIOpenGL2Render.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)
#include "ImGUIMetalRender.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
#include "ImGUIMacOSPlatform.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_IOS)
#include "ImGUIiOSPlatform.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX)
#include "ImGUIUnixPlatform.h"
#endif

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

#ifndef MENGINE_IMGUI_INI_FILE
#define MENGINE_IMGUI_INI_FILE "imgui.ini"
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

#if defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            MacOSPlatformServiceExtensionInterface * macosPlatform = PLATFORM_SERVICE()
                ->getDynamicUnknown();

            macosPlatform->removeMacOSEventHandler( m_handlerId );
            m_handlerId = INVALID_UNIQUE_ID;
        }
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_IOS)
        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            iOSPlatformServiceExtensionInterface * iosPlatform = PLATFORM_SERVICE()
                ->getDynamicUnknown();

            iosPlatform->removeIOSTouchHandler( m_handlerId );
            m_handlerId = INVALID_UNIQUE_ID;
        }
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX)
        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            UnixPlatformServiceExtensionInterface * unixPlatform = PLATFORM_SERVICE()
                ->getDynamicUnknown();

            unixPlatform->removeUnixEventHandler( m_handlerId );
            m_handlerId = INVALID_UNIQUE_ID;
        }
#endif

        size_t imIniSettingsSize = 0;
        const char * imIniSettings = ImGui::SaveIniSettingsToMemory( &imIniSettingsSize );

        const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        if( imIniSettingsSize != 0 )
        {
            OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( userFileGroup, STRINGIZE_FILEPATH_LOCAL( MENGINE_IMGUI_INI_FILE ), true, MENGINE_DOCUMENT_FACTORABLE );

            stream->write( imIniSettings, imIniSettingsSize );
            stream->flush();

            Helper::closeOutputStreamFile( userFileGroup, stream );
        }
        else
        {
            if( userFileGroup->existFile( STRINGIZE_FILEPATH_LOCAL( MENGINE_IMGUI_INI_FILE ), false ) == true )
            {
                userFileGroup->removeFile( STRINGIZE_FILEPATH_LOCAL( MENGINE_IMGUI_INI_FILE ) );
            }
        }

#if defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
        if( [MengineImGUIMacOSPlatform isInitialized] == YES )
        {
            [MengineImGUIMacOSPlatform finalizeBackend];
        }
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_IOS)
        if( [MengineImGUIiOSPlatform isInitialized] == YES )
        {
            [MengineImGUIiOSPlatform finalizeBackend];
        }
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX)
        MengineImGUIUnixPlatform_Shutdown();
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

        MengineImGUIWin32Platform_Init( hWnd );

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

            LRESULT result = MengineImGUIWin32Platform_WndProcHandler( hwnd, msg, wParam, lParam );

            return result;
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_handlerId = handlerId;
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
        MacOSPlatformServiceExtensionInterface * macosPlatform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        NSView * view = macosPlatform->getNSView();

        if( [MengineImGUIMacOSPlatform initializeWithView:view] == YES )
        {
            UniqueId handlerId = macosPlatform->addMacOSEventHandler( []( NSView * _view, NSEvent * _event )
            {
                return [MengineImGUIMacOSPlatform handleEvent:_event view:_view];
            }, MENGINE_DOCUMENT_FACTORABLE );

            m_handlerId = handlerId;
        }
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_IOS)
        iOSPlatformServiceExtensionInterface * iosPlatform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        UIView * view = iosPlatform->getUIView();

        if( [MengineImGUIiOSPlatform initializeWithView:view] == YES )
        {
            UniqueId handlerId = iosPlatform->addIOSTouchHandler( []( NSSet<UITouch *> * _touches, UIView * _view, UITouchPhase _phase )
            {
                return [MengineImGUIiOSPlatform handleTouches:_touches view:_view phase:_phase];
            }, MENGINE_DOCUMENT_FACTORABLE );

            m_handlerId = handlerId;
        }
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX)
        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            return;
        }

        if( MengineImGUIUnixPlatform_IsInitialized() == true )
        {
            MengineImGUIUnixPlatform_Shutdown();
        }

        UnixPlatformServiceExtensionInterface * unixPlatform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        if( MengineImGUIUnixPlatform_Init( unixPlatform->getX11Display(), unixPlatform->getX11Window() ) == true )
        {
            UniqueId handlerId = unixPlatform->addUnixEventHandler( []( const XEvent & _event, const Char * _text )
            {
                return MengineImGUIUnixPlatform_ProcessEvent( _event, _text );
            }, MENGINE_DOCUMENT_FACTORABLE );

            m_handlerId = handlerId;
        }
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

        MengineImGUIWin32Platform_Shutdown();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
        MacOSPlatformServiceExtensionInterface * macosPlatform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            macosPlatform->removeMacOSEventHandler( m_handlerId );
            m_handlerId = INVALID_UNIQUE_ID;
        }

        if( [MengineImGUIMacOSPlatform isInitialized] == YES )
        {
            [MengineImGUIMacOSPlatform finalizeBackend];
        }
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_IOS)
        iOSPlatformServiceExtensionInterface * iosPlatform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            iosPlatform->removeIOSTouchHandler( m_handlerId );
            m_handlerId = INVALID_UNIQUE_ID;
        }

        if( [MengineImGUIiOSPlatform isInitialized] == YES )
        {
            [MengineImGUIiOSPlatform finalizeBackend];
        }
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX)
        UnixPlatformServiceExtensionInterface * unixPlatform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        if( m_handlerId != INVALID_UNIQUE_ID )
        {
            unixPlatform->removeUnixEventHandler( m_handlerId );
            m_handlerId = INVALID_UNIQUE_ID;
        }

        MengineImGUIUnixPlatform_Shutdown();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIService::notifyRenderDeviceCreate_()
    {
#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL) && defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
        MengineImGUIOpenGL3Render_Init( "#version 150" );
#elif defined(MENGINE_ENVIRONMENT_RENDER_OPENGL) && defined(MENGINE_ENVIRONMENT_PLATFORM_IOS)
        MengineImGUIOpenGL3Render_Init( nullptr );
#elif defined(MENGINE_ENVIRONMENT_RENDER_OPENGL) && defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX)
        MengineImGUIOpenGL3Render_Init( "#version 330 core" );
#elif defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        MengineImGUIOpenGL2Render_Init();
#elif defined(MENGINE_ENVIRONMENT_RENDER_METAL)
        [MengineImGUIMetalRender initializeRenderer];
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32) && defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        DX9RenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        if( extension != nullptr )
        {
            IDirect3DDevice9 * d3dDevice = extension->getDirect3DDevice9();

            MengineImGUIDX9Render_Init( d3dDevice );
            MengineImGUIDX9Render_CreateDeviceObjects();
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIService::notifyRenderDeviceDestroy_()
    {
#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        MengineImGUIDX9Render_Shutdown();
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL) && (defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS) || defined(MENGINE_ENVIRONMENT_PLATFORM_IOS) || defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX))
        MengineImGUIOpenGL3Render_DestroyDeviceObjects();

        MengineImGUIOpenGL3Render_Shutdown();
#elif defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        MengineImGUIOpenGL2Render_DestroyDeviceObjects();

        MengineImGUIOpenGL2Render_Shutdown();
#elif defined(MENGINE_ENVIRONMENT_RENDER_METAL)
        [MengineImGUIMetalRender finalizeRenderer];
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIService::notifyRenderDeviceLostPrepare_()
    {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32) && defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        MengineImGUIDX9Render_InvalidateDeviceObjects();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIService::notifyRenderDeviceLostRestore_()
    {
#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32) && defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        MengineImGUIDX9Render_CreateDeviceObjects();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
