#include "MockupRenderFragmentShader.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderFragmentShader::MockupRenderFragmentShader()
        : m_compile( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderFragmentShader::~MockupRenderFragmentShader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MockupRenderFragmentShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderFragmentShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile )
    {
        m_name = _name;

        m_memory = _memory;
        m_compile = _compile;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderFragmentShader::finalize()
    {
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}