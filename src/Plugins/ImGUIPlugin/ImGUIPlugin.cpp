#include "ImGUIPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/Win32PlatformInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "ImGUIRender.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/DefaultPrototypeGenerator.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

//////////////////////////////////////////////////////////////////////////
extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ImGUI, Mengine::ImGUIPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
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

        ImGui::CreateContext();

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CREATE_RENDER_WINDOW, this, &ImGUIPlugin::notifyCreateRenderWindow_ );

        Win32PlatformInterface * win32Platform = PLATFORM_SERVICE()
            ->getPlatformExternal();

        uint32_t handlerId = win32Platform->addWin32ProcessHandler( []( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
        {
            LRESULT result = ImGui_ImplWin32_WndProcHandler( hwnd, msg, wParam, lParam );

            return result;
        } );

        m_handlerId = handlerId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIPlugin::_finalizePlugin()
    {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();

        ImGui::DestroyContext();

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ImGUIRender" ) );

        Win32PlatformInterface * win32Platform = PLATFORM_SERVICE()
            ->getPlatformExternal();

        win32Platform->removeWin32ProcessHandler( m_handlerId );
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIPlugin::_destroyPlugin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIPlugin::notifyCreateRenderWindow_()
    {
        void * hWnd = PLATFORM_SERVICE()
            ->getWindowHandle();

        IDirect3DDevice9 * d3dDevice = RENDER_SYSTEM()
            ->getRenderDevice();

        ImGui_ImplWin32_Init( hWnd );
        ImGui_ImplDX9_Init( d3dDevice );
        ImGui_ImplDX9_CreateDeviceObjects();
    }
}
