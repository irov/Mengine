#include "DX11RenderProgramVariable.h"

#include "DX11ErrorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderProgramVariable::DX11RenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderProgramVariable::~DX11RenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderProgramVariable::initialize( uint32_t _vertexCount, uint32_t _pixelCount )
    {
        m_vertexVariables.resize( _vertexCount );
        m_pixelVariables.resize( _pixelCount );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::finalize()
    {
        m_vertexVariables.clear();
        m_pixelVariables.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class T>
        static void makeVariableData( DX11RenderProgramVariable::ProgramVariableDesc & _variable, Vector<T> & _container, DX11RenderProgramVariable::EProgramVariableType _type, const T * _values, uint32_t _size, uint32_t _count )
        {
            _variable.type = _type;
            _variable.offset = (uint32_t)_container.size();
            _variable.size = _size;
            _variable.count = _count;

            _container.insert( _container.end(), _values, _values + _size * _count );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setVertexVariableFloats( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataFloats, EPVT_FLOAT, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setVertexVariableIntegers( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataIntegers, EPVT_INTEGER, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setVertexVariableBooleans( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataBooleans, EPVT_BOOLEAN, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setPixelVariableFloats( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_pixelFloats, EPVT_FLOAT, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setPixelVariableIntegers( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_pixelIntegers, EPVT_INTEGER, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setPixelVariableBooleans( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_pixelBooleans, EPVT_BOOLEAN, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::updatePixelVariableFloats( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        const ProgramVariableDesc & v = m_pixelVariables[_index];

        float * values = &m_dataFloats[v.offset];
        std::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::updatePixelVariableIntegers( uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        const ProgramVariableDesc & v = m_pixelVariables[_index];

        int32_t * values = &m_dataIntegers[v.offset];
        std::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::updatePixelVariableBooleans( uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        const ProgramVariableDesc & v = m_pixelVariables[_index];

        int32_t * values = &m_dataBooleans[v.offset];
        std::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderProgramVariable::apply( IDirect3DDevice9 * _pD3DDevice, const RenderProgramInterfacePtr & _program )
    {
        MENGINE_UNUSED( _program );

        uint32_t vertexEnumerator = 0;
        for( const ProgramVariableDesc & v : m_vertexVariables )
        {
            switch( v.type )
            {
            case EPVT_FLOAT:
                {
                    const float * buff = &m_dataFloats[v.offset];
                    DXCALL( _pD3DDevice, SetVertexShaderConstantF, (vertexEnumerator, buff, v.count) );
                }break;
            case EPVT_INTEGER:
                {
                    const int32_t * buff = &m_dataIntegers[v.offset];
                    DXCALL( _pD3DDevice, SetVertexShaderConstantI, (vertexEnumerator, buff, v.count) );
                }break;
            case EPVT_BOOLEAN:
                {
                    const int32_t * buff = &m_dataBooleans[v.offset];
                    DXCALL( _pD3DDevice, SetVertexShaderConstantB, (vertexEnumerator, buff, v.count) );
                }break;
            default:
                {
                    return false;
                }
            }

            ++vertexEnumerator;
        }

        uint32_t pixelEnumerator = 0;
        for( const ProgramVariableDesc & v : m_pixelVariables )
        {
            switch( v.type )
            {
            case EPVT_FLOAT:
                {
                    const float * buff = &m_pixelFloats[v.offset];
                    DXCALL( _pD3DDevice, SetPixelShaderConstantF, (pixelEnumerator, buff, v.count) );
                }break;
            case EPVT_INTEGER:
                {
                    const int32_t * buff = &m_pixelIntegers[v.offset];
                    DXCALL( _pD3DDevice, SetPixelShaderConstantI, (pixelEnumerator, buff, v.count) );
                }break;
            case EPVT_BOOLEAN:
                {
                    const int32_t * buff = &m_pixelBooleans[v.offset];
                    DXCALL( _pD3DDevice, SetPixelShaderConstantB, (pixelEnumerator, buff, v.count) );
                }break;
            default:
                {
                    return false;
                }
            }

            ++pixelEnumerator;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}