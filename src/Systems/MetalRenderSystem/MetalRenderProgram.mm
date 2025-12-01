#include "MetalRenderProgram.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderProgram::MetalRenderProgram()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderProgram::~MetalRenderProgram() = default;
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderProgram::initialize( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _attribute, uint32_t _samplerCount )
    {
        MENGINE_UNUSED( _samplerCount );

        m_name = _name;
        m_vertexShader = _vertex;
        m_fragmentShader = _fragment;
        m_vertexAttribute = _attribute;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgram::finalize()
    {
        m_vertexShader = nullptr;
        m_fragmentShader = nullptr;
        m_vertexAttribute = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MetalRenderProgram::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexAttributeInterfacePtr & MetalRenderProgram::getVertexAttribute() const
    {
        return m_vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexShaderInterfacePtr & MetalRenderProgram::getVertexShader() const
    {
        return m_vertexShader;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderFragmentShaderInterfacePtr & MetalRenderProgram::getFragmentShader() const
    {
        return m_fragmentShader;
    }
    //////////////////////////////////////////////////////////////////////////
}

