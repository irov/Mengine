#pragma once

#include "Interface/RenderVertexShaderInterface.h"
#include "Interface/MemoryInterface.h"

#include "Environment/DirectX9/DX9RenderIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderVertexShader
        : public RenderVertexShaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX9RenderVertexShader );

    public:
        DX9RenderVertexShader();
        ~DX9RenderVertexShader() override;

    public:
        const ConstString & getName() const override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _precompile );
        void finalize();

    public:
        bool compile( IDirect3DDevice9 * _pD3DDevice );
        void release();

    public:
        void enable( IDirect3DDevice9 * _pD3DDevice );

    protected:
        IDirect3DVertexShader9 * m_pD3DVertexShader;

        uint32_t m_compileReferenceCount;

        ConstString m_name;

        MemoryInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderVertexShader, RenderVertexShaderInterface> DX9RenderVertexShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}