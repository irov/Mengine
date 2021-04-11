#include "DX11RenderFragmentShader.h"

#include "DX11ErrorHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderFragmentShader::DX11RenderFragmentShader()
        : m_pD3DPixelShader( nullptr )
        , m_compile( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderFragmentShader::~DX11RenderFragmentShader()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DPixelShader == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DX11RenderFragmentShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderFragmentShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile )
    {
        m_name = _name;

        m_memory = _memory;
        m_compile = _compile;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderFragmentShader::finalize()
    {
        m_memory = nullptr;

        DXRELEASE( m_pD3DPixelShader );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderFragmentShader::compile( IDirect3DDevice9 * _pD3DDevice )
    {
        MENGINE_ASSERTION_FATAL( m_pD3DPixelShader == nullptr );

        const DWORD * shader_compile_data = m_memory->getBuffer();

        LOGGER_INFO( "render", "compile pixel shader '%s'"
            , this->getName().c_str()
        );

        IDirect3DPixelShader9 * pD3DPixelShader;
        IF_DXCALL( _pD3DDevice, CreatePixelShader, (shader_compile_data, &pD3DPixelShader) )
        {
            return false;
        }

        m_pD3DPixelShader = pD3DPixelShader;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderFragmentShader::enable( IDirect3DDevice9 * _pD3DDevice )
    {
        DXCALL( _pD3DDevice, SetPixelShader, (m_pD3DPixelShader) );
    }
    //////////////////////////////////////////////////////////////////////////
}