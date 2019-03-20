#include "DX9RenderVertexShader.h"

#include "DX9ErrorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderVertexShader::DX9RenderVertexShader()
        : m_shader( nullptr )
        , m_compile( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderVertexShader::~DX9RenderVertexShader()
    {
        if( m_shader != nullptr )
        {
            m_shader->Release();
            m_shader = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DX9RenderVertexShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile )
    {
        m_name = _name;

        m_memory = _memory;
        m_compile = _compile;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexShader::compile( IDirect3DDevice9 * _pD3DDevice )
    {
        const DWORD * dx_source = m_memory->getBuffer();

        IF_DXCALL( _pD3DDevice, CreateVertexShader, (dx_source, &m_shader) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderVertexShader::enable( IDirect3DDevice9 * _pD3DDevice )
    {
        DXCALL( _pD3DDevice, SetVertexShader, (m_shader) );
    }
}