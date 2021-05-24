#include "DX11RenderVertexShader.h"

#include "DX11ErrorHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderVertexShader::DX11RenderVertexShader()
        : m_compileReferenceCount( 0 )
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
    bool DX11RenderVertexShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _precompile )
    {
        MENGINE_UNUSED( _precompile );

        m_name = _name;

        m_memory = _memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexShader::finalize()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DVertexShader == nullptr );

        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexShader::compile( const ID3D11DevicePtr & _pD3DDevice )
    {
        if( m_compileReferenceCount == 0 )
        {
            MENGINE_ASSERTION_FATAL( m_pD3DVertexShader == nullptr );

            const DWORD * shader_compile_data = m_memory->getBuffer();
            size_t shader_compile_size = m_memory->getSize();

            LOGGER_INFO( "render", "compile vertex shader '%s'"
                , this->getName().c_str()
            );

            ID3D11VertexShader * pD3DVertexShader;
            IF_DXCALL( _pD3DDevice, CreateVertexShader, (shader_compile_data, shader_compile_size, NULL, &pD3DVertexShader) )
            {
                return false;
            }

            m_pD3DVertexShader = pD3DVertexShader;
        }

        ++m_compileReferenceCount;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexShader::release()
    {
        --m_compileReferenceCount;

        if( m_compileReferenceCount == 0 )
        {
            m_pD3DVertexShader = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexShader::enable( const ID3D11DeviceContextPtr & _pImmediateContext )
    {
        _pImmediateContext->VSSetShader( m_pD3DVertexShader.Get(), nullptr, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexShader::disable( const ID3D11DeviceContextPtr & _pImmediateContext )
    {
        _pImmediateContext->VSSetShader( nullptr, nullptr, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & DX11RenderVertexShader::getShaderCompileMemory() const
    {
        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
}