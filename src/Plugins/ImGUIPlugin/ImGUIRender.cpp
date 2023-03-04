#include "ImGUIRender.h"

#include "Interface/RenderMaterialServiceInterface.h"

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
#include "Interface/PlatformInterface.h"
#include "Interface/SDLPlatformExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
#include "Interface/DX9RenderImageExtensionInterface.h"
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
#include "Interface/OpenGLRenderImageExtensionInterface.h"
#endif

#include "Config/UInt32ToPointer.h"

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

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImGUIRender::ImGUIRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImGUIRender::~ImGUIRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImGUIRender::_activate()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRender::_deactivate()
    {
        //Empty

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRender::setProvider( const LambdaImGUIProvider & _provider )
    {
        m_provider = _provider;
    }
    //////////////////////////////////////////////////////////////////////////
    const ImGUIRender::LambdaImGUIProvider & ImGUIRender::getProvider() const
    {
        return m_provider;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRender::setImGUIRenderProvider( const ImGUIRenderProviderInterfacePtr & _renderProvider )
    {
        m_renderProvider = _renderProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const ImGUIRenderProviderInterfacePtr & ImGUIRender::getImGUIRenderProvider() const
    {
        return m_renderProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRender::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
            ->getDebugTriangleMaterial();

        _renderPipeline->addRenderExternal( _context, material, nullptr, RenderExternalInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRender::onRenderExternal() const
    {
#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        ImGui_ImplDX9_NewFrame();
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        ImGui_ImplOpenGL2_NewFrame();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
        ImGui_ImplWin32_NewFrame();
#endif

#if defined(MENGINE_ENVIRONMENT_PLATFORM_SDL)
        SDLPlatformExtensionInterface * sdlPlatform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        SDL_Window * window = sdlPlatform->getWindow();

        ImGui_ImplSDL2_NewFrame( window );
#endif

        ImGui::NewFrame();

        m_provider( m_renderProvider );

        ImGui::EndFrame();

        ImGui::Render();

        ImDrawData * imData = ImGui::GetDrawData();

        MENGINE_UNUSED( imData );

#if defined(MENGINE_ENVIRONMENT_RENDER_DIRECTX9)
        ImGui_ImplDX9_RenderDrawData( imData );
#endif

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        ImGui_ImplOpenGL2_RenderDrawData( imData );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}