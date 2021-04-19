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
    bool DX11RenderVertexShader::compile( ID3D11Device * _pD3DDevice )
    {
        MENGINE_ASSERTION_FATAL( m_pD3DVertexShader == nullptr );

        const DWORD * shader_compile_data = m_memory->getBuffer();

        LOGGER_INFO( "render", "compile vertex shader '%s'"
            , this->getName().c_str()
        );

        ID3D11VertexShader * pD3DVertexShader;
        IF_DXCALL( _pD3DDevice, CreateVertexShader, (shader_compile_data, m_memory->getSize(), NULL, &pD3DVertexShader) )
        {
            return false;
        }

        m_pD3DVertexShader = pD3DVertexShader;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexShader::enable(ID3D11DeviceContext * _pD3DDeviceContext)
    {
		_pD3DDeviceContext->VSSetShader(m_pD3DVertexShader, nullptr, 0);
	}
    //////////////////////////////////////////////////////////////////////////
}