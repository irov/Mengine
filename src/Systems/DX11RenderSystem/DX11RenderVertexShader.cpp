#include "DX11RenderVertexShader.h"

#include "DX11ErrorHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderVertexShader::DX11RenderVertexShader()
        : m_pD3DVertexShader( nullptr )
        , m_compile( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderVertexShader::~DX11RenderVertexShader()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DVertexShader == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DX11RenderVertexShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile )
    {
        m_name = _name;

        m_memory = _memory;
        m_compile = _compile;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexShader::finalize()
    {
        m_memory = nullptr;

        DXRELEASE( m_pD3DVertexShader );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexShader::compile( IDirect3DDevice9 * _pD3DDevice )
    {
        MENGINE_ASSERTION_FATAL( m_pD3DVertexShader == nullptr );

        const DWORD * dx_source = m_memory->getBuffer();

        LOGGER_INFO( "render", "compile vertex shader '%s'"
            , this->getName().c_str()
        );

        IDirect3DVertexShader9 * pD3DVertexShader;
        IF_DXCALL( _pD3DDevice, CreateVertexShader, (dx_source, &pD3DVertexShader) )
        {
            return false;
        }

        m_pD3DVertexShader = pD3DVertexShader;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexShader::enable( IDirect3DDevice9 * _pD3DDevice )
    {
        DXCALL( _pD3DDevice, SetVertexShader, (m_pD3DVertexShader) );
    }
    //////////////////////////////////////////////////////////////////////////
}