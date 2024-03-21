#include "DX9RenderFragmentShader.h"

#include "DX9RenderErrorHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderFragmentShader::DX9RenderFragmentShader()
        : m_pD3DPixelShader( nullptr )
        , m_compileReferenceCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderFragmentShader::~DX9RenderFragmentShader()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DPixelShader == nullptr, "shader is not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DX9RenderFragmentShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderFragmentShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _precompile )
    {
        MENGINE_UNUSED( _precompile );

        m_name = _name;

        m_memory = _memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderFragmentShader::finalize()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DPixelShader == nullptr, "shader is not released" );

        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderFragmentShader::compile( IDirect3DDevice9 * _pD3DDevice )
    {
        if( m_compileReferenceCount == 0 )
        {
            MENGINE_ASSERTION_FATAL( m_pD3DPixelShader == nullptr, "shader is already compile" );

            const DWORD * shader_compile_data = m_memory->getBuffer();

            LOGGER_INFO( "render", "compile pixel shader '%s'"
                , this->getName().c_str()
            );

            IDirect3DPixelShader9 * pD3DPixelShader;
            MENGINE_IF_DXCALL( _pD3DDevice, CreatePixelShader, (shader_compile_data, &pD3DPixelShader) )
            {
                return false;
            }

            m_pD3DPixelShader = pD3DPixelShader;
        }

        ++m_compileReferenceCount;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderFragmentShader::release()
    {
        --m_compileReferenceCount;

        if( m_compileReferenceCount == 0 )
        {
            MENGINE_DXRELEASE( m_pD3DPixelShader );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderFragmentShader::enable( IDirect3DDevice9 * _pD3DDevice )
    {
        MENGINE_DXCALL( _pD3DDevice, SetPixelShader, (m_pD3DPixelShader) );
    }
    //////////////////////////////////////////////////////////////////////////
}