#pragma once

#include "Interface/UnknownInterface.h"

#include "d3d9.h"

namespace Mengine
{
    class DX9RenderSystemExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual IDirect3DDevice9 * getDirect3DDevice9() const = 0;
    };
}