#include "ImGUIMetalRenderBridge.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)

#import "Environment/Metal/MetalRenderImageExtensionInterface.h"
#import "Environment/Metal/MetalRenderSystemExtensionInterface.h"

#include "Interface/RenderSystemInterface.h"

#include "Kernel/Logger.h"

#include "imgui_impl_metal.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool ImGUIMetalRenderBridge_initialize()
        {
            MetalRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
                ->getUnknown();

            if( extension == nullptr )
            {
                LOGGER_ERROR( "invalid get MetalRenderSystemExtensionInterface" );

                return false;
            }

            id<MTLDevice> device = extension->getMetalDevice();

            if( device == nil )
            {
                LOGGER_ERROR( "invalid get Metal device for ImGUI" );

                return false;
            }

            bool successful = ImGui_ImplMetal_Init( device );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        void ImGUIMetalRenderBridge_finalize()
        {
            ImGuiIO & io = ImGui::GetIO();

            if( io.BackendRendererUserData != nullptr )
            {
                ImGui_ImplMetal_Shutdown();
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void ImGUIMetalRenderBridge_newFrame()
        {
            MetalRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
                ->getUnknown();

            if( extension == nullptr )
            {
                return;
            }

            MTLRenderPassDescriptor * renderPassDescriptor = extension->getMetalRenderPassDescriptor();

            if( renderPassDescriptor == nil )
            {
                return;
            }

            ImGuiIO & io = ImGui::GetIO();

            if( io.BackendRendererUserData == nullptr )
            {
                return;
            }

            ImGui_ImplMetal_NewFrame( renderPassDescriptor );
        }
        //////////////////////////////////////////////////////////////////////////
        void ImGUIMetalRenderBridge_renderDrawData( ImDrawData * _drawData )
        {
            MetalRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
                ->getUnknown();

            if( extension == nullptr )
            {
                return;
            }

            id<MTLCommandBuffer> commandBuffer = extension->getMetalCommandBuffer();
            id<MTLRenderCommandEncoder> renderCommandEncoder = extension->getMetalRenderCommandEncoder();

            if( commandBuffer == nil || renderCommandEncoder == nil )
            {
                return;
            }

            ImGui_ImplMetal_RenderDrawData( _drawData, commandBuffer, renderCommandEncoder );
        }
        //////////////////////////////////////////////////////////////////////////
        ImTextureID ImGUIMetalRenderBridge_getTexture( const RenderTextureInterfacePtr & _texture )
        {
            const RenderImageInterfacePtr & image = _texture->getImage();

            MetalRenderImageExtensionInterface * extension = image->getUnknown();

            id<MTLTexture> texture = extension->getMetalTexture();

            return (ImTextureID)(uintptr_t)(__bridge void *)texture;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}

#endif
