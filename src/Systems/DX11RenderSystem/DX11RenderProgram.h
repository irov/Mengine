#pragma once

#include "Interface/RenderProgramInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "DX11RenderVertexShader.h"
#include "DX11RenderFragmentShader.h"
#include "DX11RenderVertexAttribute.h"

#include "math/uv4.h"
#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderProgram
        : public RenderProgramInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX11RenderProgram );

    public:
        DX11RenderProgram();
        ~DX11RenderProgram() override;

    public:
        const ConstString & getName() const override;

    public:
        const RenderVertexAttributeInterfacePtr & getVertexAttribute() const override;
        const RenderVertexShaderInterfacePtr & getVertexShader() const override;
        const RenderFragmentShaderInterfacePtr & getFragmentShader() const override;

    public:
        bool initialize( const ConstString & _name, const DX11RenderVertexShaderPtr & _vertexShader, const DX11RenderFragmentShaderPtr & _fragmentShader, const RenderVertexAttributeInterfacePtr & _vertexAttribute );
        void finalize();

    public:
        bool compile( const ID3D11DevicePtr & _pD3DDevice );
        void release();

    public:
        void enable( const ID3D11DeviceContextPtr & _pImmediateContext );
        void disable( const ID3D11DeviceContextPtr & _pImmediateContext );

    public:
        // b0 - first buffer must be float4x4 
        void bindMatrix( const ID3D11DeviceContextPtr & _pImmediateContext, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalPMWInvMatrix );

    protected:
        ConstString m_name;

        DX11RenderVertexShaderPtr m_vertexShader;
        DX11RenderFragmentShaderPtr m_fragmentShader;
        DX11RenderVertexAttributePtr m_vertexAttribute;

        ID3D11BufferPtr m_bindMatrixBuffer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderProgram, RenderProgramInterface> DX11RenderProgramPtr;
    //////////////////////////////////////////////////////////////////////////
}