#include "MockupRenderVertexShader.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderVertexShader::MockupRenderVertexShader()
        : m_compile( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderVertexShader::~MockupRenderVertexShader()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MockupRenderVertexShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderVertexShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile )
    {
        m_name = _name;

        m_memory = _memory;
        m_compile = _compile;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderVertexShader::finalize()
    {
        m_memory = nullptr;
    }
}