#include "DX9RenderVertexShader.h"

#include "DX9ErrorHelper.h"

#include "Kernel/Logger.h"

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
        this->finalize();
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
    void DX9RenderVertexShader::finalize()
    {
        m_memory = nullptr;

        DXRELEASE( m_shader );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexShader::compile( IDirect3DDevice9 * _pD3DDevice )
    {
        MENGINE_ASSERTION_FATAL( m_shader == nullptr );

        const DWORD * dx_source = m_memory->getBuffer();

        LOGGER_INFO( "compile vertex shader '%s'"
            , this->getName().c_str()
        );

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