#pragma once

#include "Interface/RenderFragmentShaderInterface.h"
#include "Interface/MemoryInterface.h"

#include "Environment/DirectX11/DX11RenderIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderFragmentShader
        : public RenderFragmentShaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX11RenderFragmentShader );

    public:
        DX11RenderFragmentShader();
        ~DX11RenderFragmentShader() override;

    public:
        const ConstString & getName() const override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _precompile );
        void finalize();

    public:
        bool compile( const ID3D11DevicePtr & _pD3DDevice );
        void release();

    public:
        void enable( const ID3D11DeviceContextPtr & _pD3DDeviceContext );
        void disable( const ID3D11DeviceContextPtr & _pD3DDeviceContext );

    protected:
        ID3D11PixelShaderPtr m_pD3DPixelShader;

        uint32_t m_compileReferenceCount;

        ConstString m_name;

        MemoryInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderFragmentShader, RenderFragmentShaderInterface> DX11RenderFragmentShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}