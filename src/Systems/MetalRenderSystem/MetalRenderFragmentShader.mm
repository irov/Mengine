#include "MetalRenderFragmentShader.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderFragmentShader::MetalRenderFragmentShader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderFragmentShader::~MetalRenderFragmentShader() = default;
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderFragmentShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile )
    {
        MENGINE_UNUSED( _compile );

        m_name = _name;
        m_memory = _memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderFragmentShader::finalize()
    {
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MetalRenderFragmentShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
}

