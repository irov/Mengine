#pragma once

#include "Interface/RenderTextureInterface.h"

#include "imgui.h"

namespace Mengine
{
    namespace Helper
    {
        bool ImGUIMetalRenderBridge_initialize();
        void ImGUIMetalRenderBridge_finalize();
        void ImGUIMetalRenderBridge_newFrame();
        void ImGUIMetalRenderBridge_renderDrawData( ImDrawData * _drawData );
        ImTextureID ImGUIMetalRenderBridge_getTexture( const RenderTextureInterfacePtr & _texture );
    }
}
