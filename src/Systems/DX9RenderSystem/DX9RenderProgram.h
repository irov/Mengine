#pragma once

#include "Interface/RenderSystemInterface.h"

#include "DX9RenderVertexShader.h"
#include "DX9RenderFragmentShader.h"
#include "DX9RenderVertexAttribute.h"

#include "d3d9.h"

namespace Mengine
{
    class DX9RenderProgram
        : public RenderProgramInterface
        , public Factorable
    {
    public:
        DX9RenderProgram();
        ~DX9RenderProgram() override;

    public:
        const ConstString & getName() const override;

    public:
        RenderVertexAttributeInterfacePtr getVertexAttribute() const override;
        RenderVertexShaderInterfacePtr getVertexShader() const override;
        RenderFragmentShaderInterfacePtr getFragmentShader() const override;

    public:
        bool initialize( const ConstString & _name, const DX9RenderVertexShaderPtr & _vertexShader, const DX9RenderFragmentShaderPtr & _fragmentShader, const RenderVertexAttributeInterfacePtr & _vertexAttribute );

    public:
        bool compile( IDirect3DDevice9 * _pD3DDevice );

    public:
        void enable( IDirect3DDevice9 * _pD3DDevice );

    public:
        void bindTextureMask( IDirect3DDevice9 * _pD3DDevice, const mt::uv4f * _textureMasks );
        void bindMatrix( IDirect3DDevice9 * _pD3DDevice, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalPMWInvMatrix );

    protected:
        ConstString m_name;

        DX9RenderVertexShaderPtr m_vertexShader;
        DX9RenderFragmentShaderPtr m_fragmentShader;
        DX9RenderVertexAttributePtr m_vertexAttribute;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderProgram> DX9RenderProgramPtr;
}