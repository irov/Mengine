#include "ImGUIRenderProvider.h"

#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)
#include "ImGUIMetalRenderBridge.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL2)
#include "Environment/SDL2/SDL2PlatformServiceExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL3)
#include "Environment/SDL3/SDL3PlatformServiceExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
#include "Environment/MacOS/MacOSPlatformServiceExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
#include "Environment/DirectX9/DX9RenderImageExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
#include "Environment/OpenGL/OpenGLRenderImageExtensionInterface.h"
#endif

#include "Config/UInt32ToPointer.h"

#include "imgui.h"

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
#include "imgui_impl_win32.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL2)
#include "imgui_impl_sdl2.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL3)
#include "imgui_impl_sdl3.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
#include "imgui_impl_osx.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
#include "imgui_impl_dx9.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL) && defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
#include "imgui_impl_opengl3.h"
#elif defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
#include "imgui_impl_opengl2.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImGUIRenderProvider::ImGUIRenderProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImGUIRenderProvider::~ImGUIRenderProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImTextureID ImGUIRenderProvider::getImTexture( const RenderTextureInterfacePtr & _texture ) const
    {
        const RenderImageInterfacePtr & image = _texture->getImage();

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        DX9RenderImageExtensionInterface * extension = image->getUnknown();

        IDirect3DTexture9 * pD3DTexture = extension->getD3DTexture();

        return (ImTextureID)pD3DTexture;
#elif defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        OpenGLRenderImageExtensionInterface * extension = image->getUnknown();

        GLuint UID = extension->getUID();

        return MENGINE_UINT32_TO_POINTER( ImTextureID, UID );
#elif defined(MENGINE_ENVIRONMENT_RENDER_METAL)
        return Helper::ImGUIMetalRenderBridge_getTexture( _texture );
#else
        MENGINE_UNUSED( image );

        return (ImTextureID)nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRenderProvider::newFrame()
    {
#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        ImGui_ImplDX9_NewFrame();
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL) && defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
        ImGui_ImplOpenGL3_NewFrame();
#elif defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        ImGui_ImplOpenGL2_NewFrame();
#elif defined(MENGINE_ENVIRONMENT_RENDER_METAL)
        Helper::ImGUIMetalRenderBridge_newFrame();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        ImGui_ImplWin32_NewFrame();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL2)
        ImGui_ImplSDL2_NewFrame();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL3)
        ImGui_ImplSDL3_NewFrame();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
        MacOSPlatformServiceExtensionInterface * macosPlatform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        ImGui_ImplOSX_NewFrame( macosPlatform->getNSView() );
#endif

        ImGui::NewFrame();
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRenderProvider::endFrame()
    {
        ImGui::EndFrame();

        ImGui::Render();

        ImDrawData * imData = ImGui::GetDrawData();

        MENGINE_UNUSED( imData );

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        ImGui_ImplDX9_RenderDrawData( imData );
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL) && defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
        ImGui_ImplOpenGL3_RenderDrawData( imData );
#elif defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        ImGui_ImplOpenGL2_RenderDrawData( imData );
#elif defined(MENGINE_ENVIRONMENT_RENDER_METAL)
        Helper::ImGUIMetalRenderBridge_renderDrawData( imData );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
