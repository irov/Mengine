#pragma once

#include "Interface/RenderProgramInterface.h"

#include "MockupRenderVertexShader.h"
#include "MockupRenderFragmentShader.h"
#include "MockupRenderVertexAttribute.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderProgram
        : public RenderProgramInterface
        , public Factorable
    {
    public:
        MockupRenderProgram();
        ~MockupRenderProgram() override;

    public:
        bool initialize( const ConstString & _name, const MockupRenderVertexShaderPtr & _vertexShader, const MockupRenderFragmentShaderPtr & _fragmentShader, const RenderVertexAttributeInterfacePtr & _vertexAttribute );
        void finalize();

    public:
        const ConstString & getName() const override;

    public:
        const RenderVertexAttributeInterfacePtr & getVertexAttribute() const override;
        const RenderVertexShaderInterfacePtr & getVertexShader() const override;
        const RenderFragmentShaderInterfacePtr & getFragmentShader() const override;

    protected:
        ConstString m_name;

        MockupRenderVertexShaderPtr m_vertexShader;
        MockupRenderFragmentShaderPtr m_fragmentShader;
        MockupRenderVertexAttributePtr m_vertexAttribute;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderProgram> MockupRenderProgramPtr;
    //////////////////////////////////////////////////////////////////////////
}