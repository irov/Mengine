#pragma once

#include "Interface/RenderProgramInterface.h"

#include "Environment/DirectX9/DX9RenderIncluder.h"

#include "DX9RenderVertexShader.h"
#include "DX9RenderFragmentShader.h"
#include "DX9RenderVertexAttribute.h"

#include "Kernel/ReferenceCounter.h"

#include "math/uv4.h"
#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderProgram
        : public RenderProgramInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX9RenderProgram );

    public:
        DX9RenderProgram();
        ~DX9RenderProgram() override;

    public:
        const ConstString & getName() const override;

    public:
        const RenderVertexAttributeInterfacePtr & getVertexAttribute() const override;
        const RenderVertexShaderInterfacePtr & getVertexShader() const override;
        const RenderFragmentShaderInterfacePtr & getFragmentShader() const override;

    public:
        bool initialize( const ConstString & _name, const DX9RenderVertexShaderPtr & _vertexShader, const DX9RenderFragmentShaderPtr & _fragmentShader, const RenderVertexAttributeInterfacePtr & _vertexAttribute );
        void finalize();

    public:
        void setDeferredCompile( bool _deferredCompile );
        bool getDeferredCompile() const;

    public:
        bool compile( IDirect3DDevice9 * _pD3DDevice );
        void release();

    public:
        void enable( IDirect3DDevice9 * _pD3DDevice );
        void disable( IDirect3DDevice9 * _pD3DDevice );

    public:
        void bindMatrix( IDirect3DDevice9 * _pD3DDevice, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalPMWInvMatrix );

    protected:
        ConstString m_name;

        ReferenceCounter m_compileReferenceCount;

        DX9RenderVertexShaderPtr m_vertexShader;
        DX9RenderFragmentShaderPtr m_fragmentShader;
        DX9RenderVertexAttributePtr m_vertexAttribute;

        bool m_deferredCompile;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderProgram, RenderProgramInterface> DX9RenderProgramPtr;
    //////////////////////////////////////////////////////////////////////////
}