#pragma once

#include "Interface/RenderProgramVariableInterface.h"
#include "Interface/RenderProgramInterface.h"

#include "Environment/DirectX9/DX9RenderIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderProgramVariableInterface
        : public RenderProgramVariableInterface
    {
    public:
        virtual bool initialize( uint32_t _vertexCount, uint32_t _pixelCount ) = 0;
        virtual void finalize() = 0;

    public:
        virtual bool apply( IDirect3DDevice9 * _pD3DDevice, const RenderProgramInterfacePtr & _program ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderProgramVariableInterface, RenderProgramVariableInterface> DX9RenderProgramVariableInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}