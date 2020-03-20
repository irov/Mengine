#include "ImGUIPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "ImGUIRender.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/DefaultPrototypeGenerator.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ImGUI, Mengine::ImGUIPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImGUIPlugin::ImGUIPlugin()
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
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ImGUI" ), Helper::makeFactorableUnique<NodePrototypeGenerator<ImGUIRender, 16>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        ImGui::CreateContext();

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CREATE_RENDER_WINDOW, this, &ImGUIPlugin::notifyCreateRenderWindow_ );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIPlugin::_finalizePlugin()
    {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();

        ImGui::DestroyContext();

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ImGUI" ) );
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
    }
}
