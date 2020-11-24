#include "MockupRenderProgram.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderProgram::MockupRenderProgram()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderProgram::~MockupRenderProgram()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderProgram::initialize( const ConstString & _name, const MockupRenderVertexShaderPtr & _vertexShader, const MockupRenderFragmentShaderPtr & _fragmentShader, const RenderVertexAttributeInterfacePtr & _vertexAttribute )
    {
        m_name = _name;

        m_vertexShader = _vertexShader;
        m_fragmentShader = _fragmentShader;
        m_vertexAttribute = _vertexAttribute;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgram::finalize()
    {
        m_vertexShader = nullptr;
        m_fragmentShader = nullptr;
        m_vertexAttribute = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MockupRenderProgram::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr MockupRenderProgram::getVertexAttribute() const
    {
        return m_vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr MockupRenderProgram::getVertexShader() const
    {
        return m_vertexShader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr MockupRenderProgram::getFragmentShader() const
    {
        return m_fragmentShader;
    }
    //////////////////////////////////////////////////////////////////////////
}