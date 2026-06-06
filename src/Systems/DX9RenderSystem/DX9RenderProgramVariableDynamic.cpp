#include "DX9RenderProgramVariableDynamic.h"

#include "DX9RenderErrorHelper.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderProgramVariableDynamic::DX9RenderProgramVariableDynamic()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderProgramVariableDynamic::~DX9RenderProgramVariableDynamic()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderProgramVariableDynamic::initialize( uint32_t _vertexCount, uint32_t _pixelCount )
    {
        m_vertexVariables.resize( _vertexCount );
        m_pixelVariables.resize( _pixelCount );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariableDynamic::finalize()
    {
        m_vertexVariables.clear();
        m_pixelVariables.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariableDynamic::setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        if( _index >= (uint32_t)m_vertexVariables.size() )
        {
            m_vertexVariables.resize( _index + 1 );
        }

        ProgramVariableDesc & v = m_vertexVariables[_index];
        v.size = _size;
        v.count = _count;
        v.regIndex = _index;
        v.data.assign( _values, _values + _size * _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariableDynamic::setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        if( _index >= (uint32_t)m_pixelVariables.size() )
        {
            m_pixelVariables.resize( _index + 1 );
        }

        ProgramVariableDesc & v = m_pixelVariables[_index];
        v.size = _size;
        v.count = _count;
        v.regIndex = _index;
        v.data.assign( _values, _values + _size * _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariableDynamic::updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        if( _index >= (uint32_t)m_pixelVariables.size() )
        {
            return;
        }

        ProgramVariableDesc & v = m_pixelVariables[_index];

        MENGINE_UNUSED( _size );
        MENGINE_UNUSED( _count );

        StdAlgorithm::copy( _values, _values + v.size * v.count, v.data.data() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderProgramVariableDynamic::apply( IDirect3DDevice9 * _pD3DDevice, const RenderProgramInterfacePtr & _program )
    {
        MENGINE_UNUSED( _program );

        for( const ProgramVariableDesc & v : m_vertexVariables )
        {
            if( v.count == 0 )
            {
                continue;
            }

            MENGINE_IF_DX9_CALL( _pD3DDevice, SetVertexShaderConstantF, (v.regIndex, v.data.data(), v.count) )
            {
                return false;
            }
        }

        for( const ProgramVariableDesc & v : m_pixelVariables )
        {
            if( v.count == 0 )
            {
                continue;
            }

            MENGINE_IF_DX9_CALL( _pD3DDevice, SetPixelShaderConstantF, (v.regIndex, v.data.data(), v.count) )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
