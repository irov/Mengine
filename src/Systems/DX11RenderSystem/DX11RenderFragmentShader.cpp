#include "DX11RenderFragmentShader.h"

#include "DX11ErrorHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderFragmentShader::DX11RenderFragmentShader()
        : m_compileReferenceCount( 0 )
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
    bool DX11RenderFragmentShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _precompile )
    {
        MENGINE_UNUSED( _precompile );

        m_name = _name;

        m_memory = _memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderFragmentShader::finalize()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DPixelShader == nullptr );

        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderFragmentShader::compile( const ID3D11DevicePtr & _pD3DDevice )
    {
        if( m_compileReferenceCount == 0 )
        {
            MENGINE_ASSERTION_FATAL( m_pD3DPixelShader == nullptr );

            const DWORD * shader_compile_data = m_memory->getBuffer();
            size_t shader_compile_size = m_memory->getSize();

            LOGGER_INFO( "render", "compile pixel shader '%s'"
                , this->getName().c_str()
            );

            ID3D11PixelShader * pD3DPixelShader;
            IF_DXCALL( _pD3DDevice, CreatePixelShader, (shader_compile_data, shader_compile_size, NULL, &pD3DPixelShader) )
            {
                return false;
            }

            m_pD3DPixelShader.Attach( pD3DPixelShader );
        }

        ++m_compileReferenceCount;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderFragmentShader::release()
    {
        --m_compileReferenceCount;

        if( m_compileReferenceCount == 0 )
        {
            m_pD3DPixelShader = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderFragmentShader::enable( const ID3D11DeviceContextPtr & _pD3DDeviceContext )
    {
        _pD3DDeviceContext->PSSetShader( m_pD3DPixelShader.Get(), nullptr, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderFragmentShader::disable( const ID3D11DeviceContextPtr & _pD3DDeviceContext )
    {
        _pD3DDeviceContext->PSSetShader( nullptr, nullptr, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
}