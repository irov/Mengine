#include "ImGUIRenderProvider.h"

#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)
#include "ImGUIMetalRender.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
#include "ImGUIMacOSPlatform.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_IOS)
#include "ImGUIiOSPlatform.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
#include "Environment/MacOS/MacOSPlatformServiceExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_IOS)
#include "Environment/iOS/iOSPlatformServiceExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
#include "Environment/DirectX9/DX9RenderImageExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
#include "Environment/OpenGL/OpenGLRenderImageExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX)
#include "ImGUIUnixPlatform.h"
#endif

#include "Config/UInt32ToPointer.h"

#include "imgui.h"

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
        return [MengineImGUIMetalRender textureForRenderTexture:_texture];
#else
        MENGINE_UNUSED( image );

        return (ImTextureID)nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRenderProvider::newFrame()
    {
#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        MengineImGUIDX9Render_NewFrame();
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL) && (defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS) || defined(MENGINE_ENVIRONMENT_PLATFORM_IOS) || defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX))
        MengineImGUIOpenGL3Render_NewFrame();
#elif defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        MengineImGUIOpenGL2Render_NewFrame();
#elif defined(MENGINE_ENVIRONMENT_RENDER_METAL)
        [MengineImGUIMetalRender newFrame];
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        MengineImGUIWin32Platform_NewFrame();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
        MacOSPlatformServiceExtensionInterface * macosPlatform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        [MengineImGUIMacOSPlatform newFrameWithView:macosPlatform->getNSView()];
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_IOS)
        iOSPlatformServiceExtensionInterface * iosPlatform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        [MengineImGUIiOSPlatform newFrameWithView:iosPlatform->getUIView()];
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX)
        MengineImGUIUnixPlatform_NewFrame();
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
        MengineImGUIDX9Render_RenderDrawData( imData );
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL) && (defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS) || defined(MENGINE_ENVIRONMENT_PLATFORM_IOS) || defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX))
        MengineImGUIOpenGL3Render_RenderDrawData( imData );
#elif defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        MengineImGUIOpenGL2Render_RenderDrawData( imData );
#elif defined(MENGINE_ENVIRONMENT_RENDER_METAL)
        [MengineImGUIMetalRender renderDrawData:imData];
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
