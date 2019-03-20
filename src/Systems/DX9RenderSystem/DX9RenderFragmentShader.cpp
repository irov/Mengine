#include "DX9RenderFragmentShader.h"

#include "DX9ErrorHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderFragmentShader::DX9RenderFragmentShader()
        : m_shader( nullptr )
        , m_compile( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderFragmentShader::~DX9RenderFragmentShader()
    {
        if( m_shader != nullptr )
        {
            m_shader->Release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DX9RenderFragmentShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderFragmentShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile )
    {
        m_name = _name;

        m_memory = _memory;
        m_compile = _compile;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderFragmentShader::compile( IDirect3DDevice9 * _pD3DDevice )
    {
        const DWORD * shader_compile_data = m_memory->getBuffer();

        IF_DXCALL( _pD3DDevice, CreatePixelShader, (shader_compile_data, &m_shader) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderFragmentShader::enable( IDirect3DDevice9 * _pD3DDevice )
    {
        DXCALL( _pD3DDevice, SetPixelShader, (m_shader) );
    }
}