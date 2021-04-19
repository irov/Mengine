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
    class DX11RenderProgram
        : public RenderProgramInterface
        , public Factorable
    {
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
        bool compile( ID3D11Device * _pD3DDevice );

    public:
        void enable( ID3D11Device * _pD3DDevice );
        void disable( ID3D11Device * _pD3DDevice );

    public:
        void bindTextureMask( ID3D11Device * _pD3DDevice, const mt::uv4f * _textureMasks );
        void bindMatrix( ID3D11Device * _pD3DDevice, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalPMWInvMatrix );

    protected:
        ConstString m_name;

        DX11RenderVertexShaderPtr m_vertexShader;
        DX11RenderFragmentShaderPtr m_fragmentShader;
        DX11RenderVertexAttributePtr m_vertexAttribute;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderProgram, RenderProgramInterface> DX11RenderProgramPtr;
    //////////////////////////////////////////////////////////////////////////
}