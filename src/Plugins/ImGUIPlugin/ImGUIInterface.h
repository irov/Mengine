#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/RenderTextureInterface.h"

#include "Config/Lambda.h"

#include "imgui.h"

namespace Mengine
{
    class ImGUIRenderProviderInterface
        : public Mixin
    {
    public:
        virtual ImTextureID getImTexture( const RenderTextureInterfacePtr & _texture ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ImGUIRenderProviderInterface> ImGUIRenderProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownImGUIRender
        : public UnknownInterface
    {
    public:
        typedef Lambda<void( const ImGUIRenderProviderInterfacePtr & )> LambdaImGUIProvider;

        virtual void setProvider( const LambdaImGUIProvider & _provider ) = 0;
        virtual const LambdaImGUIProvider & getProvider() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
