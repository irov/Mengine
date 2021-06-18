#include "DX9RenderProgramVariable.h"

#include "DX9RenderErrorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class T>
        static void makeVariableData( DX9RenderProgramVariable::ProgramVariableDesc & _variable, Vector<T> & _container, const T * _values, uint32_t _size, uint32_t _count )
        {
            _variable.offset = (uint32_t)_container.size();
            _variable.size = _size;
            _variable.count = _count;

            _container.insert( _container.end(), _values, _values + _size * _count );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderProgramVariable::DX9RenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderProgramVariable::~DX9RenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderProgramVariable::initialize( uint32_t _vertexCount, uint32_t _pixelCount )
    {
        m_vertexVariables.resize( _vertexCount );
        m_pixelVariables.resize( _pixelCount );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariable::finalize()
    {
        m_dataFloats.clear();
        m_pixelFloats.clear();

        m_vertexVariables.clear();
        m_pixelVariables.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariable::setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataFloats, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariable::setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_pixelFloats, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariable::updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        const ProgramVariableDesc & v = m_pixelVariables[_index];

        float * values = m_pixelFloats.data() + v.offset;
        std::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderProgramVariable::apply( IDirect3DDevice9 * _pD3DDevice, const RenderProgramInterfacePtr & _program )
    {
        MENGINE_UNUSED( _program );

        uint32_t vertexEnumerator = 0;
        for( const ProgramVariableDesc & v : m_vertexVariables )
        {
            const float * buff = m_dataFloats.data() + v.offset;
            IF_DXCALL( _pD3DDevice, SetVertexShaderConstantF, (vertexEnumerator, buff, v.count) )
            {
                return false;
            }

            ++vertexEnumerator;
        }

        uint32_t pixelEnumerator = 0;
        for( const ProgramVariableDesc & v : m_pixelVariables )
        {
            const float * buff = m_pixelFloats.data() + v.offset;
            IF_DXCALL( _pD3DDevice, SetPixelShaderConstantF, (pixelEnumerator, buff, v.count) )
            {
                return false;
            }

            ++pixelEnumerator;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}