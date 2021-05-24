#pragma once

#include "Interface/RenderVertexShaderInterface.h"
#include "Interface/MemoryInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderVertexShader
        : public RenderVertexShaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX11RenderVertexShader );

    public:
        DX11RenderVertexShader();
        ~DX11RenderVertexShader() override;

    public:
        const ConstString & getName() const override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _precompile );
        void finalize();

    public:
        bool compile( const ID3D11DevicePtr & _pD3DDevice );
        void release();

    public:
        void enable( const ID3D11DeviceContextPtr & _pImmediateContext );
        void disable( const ID3D11DeviceContextPtr & _pImmediateContext );

        const MemoryInterfacePtr & getShaderCompileMemory() const;

    protected:
        ID3D11VertexShaderPtr m_pD3DVertexShader;

        uint32_t m_compileReferenceCount;

        ConstString m_name;

        MemoryInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderVertexShader, RenderVertexShaderInterface> DX11RenderVertexShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}