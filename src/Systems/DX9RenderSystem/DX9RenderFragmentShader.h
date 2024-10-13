#pragma once

#include "Interface/RenderFragmentShaderInterface.h"
#include "Interface/MemoryInterface.h"

#include "Environment/DirectX9/DX9RenderIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/ReferenceCounter.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderFragmentShader
        : public RenderFragmentShaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX9RenderFragmentShader );

    public:
        DX9RenderFragmentShader();
        ~DX9RenderFragmentShader() override;

    public:
        const ConstString & getName() const override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile );
        void finalize();

    public:
        bool compile( IDirect3DDevice9 * _pD3DDevice );
        void release();

    public:
        void enable( IDirect3DDevice9 * _pD3DDevice );

    protected:
        IDirect3DPixelShader9 * m_pD3DPixelShader;

        ReferenceCounter m_compileReferenceCount;

        ConstString m_name;

        MemoryInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderFragmentShader, RenderFragmentShaderInterface> DX9RenderFragmentShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}