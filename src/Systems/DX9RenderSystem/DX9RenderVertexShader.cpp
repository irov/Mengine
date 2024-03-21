#include "DX9RenderVertexShader.h"

#include "DX9RenderErrorHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderVertexShader::DX9RenderVertexShader()
        : m_pD3DVertexShader( nullptr )
        , m_compileReferenceCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderVertexShader::~DX9RenderVertexShader()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DVertexShader == nullptr, "shader not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DX9RenderVertexShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _precompile )
    {
        MENGINE_UNUSED( _precompile );

        m_name = _name;

        m_memory = _memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderVertexShader::finalize()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DVertexShader == nullptr, "shader '%s' not released"
            , this->getName().c_str()
        );
        
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexShader::compile( IDirect3DDevice9 * _pD3DDevice )
    {
        if( m_compileReferenceCount == 0 )
        {
            MENGINE_ASSERTION_FATAL( m_pD3DVertexShader == nullptr, "shader '%s' already compiled"
                , this->getName().c_str()
            );

            const DWORD * dx_source = m_memory->getBuffer();

            LOGGER_INFO( "render", "compile vertex shader '%s'"
                , this->getName().c_str()
            );

            IDirect3DVertexShader9 * pD3DVertexShader;
            MENGINE_IF_DXCALL( _pD3DDevice, CreateVertexShader, (dx_source, &pD3DVertexShader) )
            {
                return false;
            }

            m_pD3DVertexShader = pD3DVertexShader;
        }

        ++m_compileReferenceCount;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderVertexShader::release()
    {
        --m_compileReferenceCount;

        if( m_compileReferenceCount == 0 )
        {
            MENGINE_DXRELEASE( m_pD3DVertexShader );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderVertexShader::enable( IDirect3DDevice9 * _pD3DDevice )
    {
        MENGINE_DXCALL( _pD3DDevice, SetVertexShader, (m_pD3DVertexShader) );
    }
    //////////////////////////////////////////////////////////////////////////
}