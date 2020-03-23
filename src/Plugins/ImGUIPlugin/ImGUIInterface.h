#pragma once

#include "Interface/UnknownInterface.h"

#include "Config/Lambda.h"

#include "imgui.h"

namespace Mengine
{
    class UnknownImGUIRender
        : public UnknownInterface
    {
    public:
        typedef Lambda<void()> LambdaImGUIProvider;

        virtual void setProvider( const LambdaImGUIProvider & _provider ) = 0;
        virtual const LambdaImGUIProvider & getProvider() const = 0;
    };
}
