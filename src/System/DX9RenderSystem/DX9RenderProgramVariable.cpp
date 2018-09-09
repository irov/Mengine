#include "DX9RenderProgramVariable.h"

#include "DX9ErrorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderProgramVariable::DX9RenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderProgramVariable::~DX9RenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderProgramVariable::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariable::finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class T>
        void makeVariableData( DX9RenderProgramVariable::Variable & _variable, Vector<T> & _container, uint32_t _type, uint32_t _index, T * _values, uint32_t _count )
        {
            _variable.index = _index;
            _variable.type = _type;
            _variable.offset = _container.size();
            _variable.count = _count;

            _container.insert( _container.end(), _values, _values + _count );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariable::setVertexVariableFloats( uint32_t _index, float * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataFloats, 0, _index, _values, _count );

        m_vertexVariables.push_back( v );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariable::setVertexVariableIntegers( uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataIntegers, 1, _index, _values, _count );

        m_vertexVariables.push_back( v );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariable::setVertexVariableBooleans( uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataBooleans, 2, _index, _values, _count );

        m_vertexVariables.push_back( v );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariable::setPixelVariableFloats( uint32_t _index, float * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataFloats, 0, _index, _values, _count );

        m_pixelVariables.push_back( v );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariable::setPixelVariableIntegers( uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataIntegers, 1, _index, _values, _count );

        m_pixelVariables.push_back( v );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderProgramVariable::setPixelVariableBooleans( uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataBooleans, 2, _index, _values, _count );

        m_pixelVariables.push_back( v );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderProgramVariable::apply( IDirect3DDevice9 * _pD3DDevice, const RenderProgramInterfacePtr & _program )
    {
        (void)_program;

        for( const Variable & v : m_vertexVariables )
        {
            switch( v.type )
            {
            case 0:
                {
                    const float * buff = &m_dataFloats[v.offset];
                    DXCALL( _pD3DDevice, SetVertexShaderConstantF, (v.index, buff, v.count) );
                }break;
            case 1:
                {
                    const int32_t * buff = &m_dataIntegers[v.offset];
                    DXCALL( _pD3DDevice, SetVertexShaderConstantI, (v.index, buff, v.count) );
                }break;
            case 2:
                {
                    const int32_t * buff = &m_dataBooleans[v.offset];
                    DXCALL( _pD3DDevice, SetVertexShaderConstantB, (v.index, buff, v.count) );
                }break;
            default:
                {
                    return false;
                }
            }
        }

        for( const Variable & v : m_pixelVariables )
        {
            switch( v.type )
            {
            case 0:
                {
                    const float * buff = &m_dataFloats[v.offset];
                    DXCALL( _pD3DDevice, SetPixelShaderConstantF, (v.index, buff, v.count) );
                }break;
            case 1:
                {
                    const int32_t * buff = &m_dataIntegers[v.offset];
                    DXCALL( _pD3DDevice, SetPixelShaderConstantI, (v.index, buff, v.count) );
                }break;
            case 2:
                {
                    const int32_t * buff = &m_dataBooleans[v.offset];
                    DXCALL( _pD3DDevice, SetPixelShaderConstantB, (v.index, buff, v.count) );
                }break;
            default:
                {
                    return false;
                }
            }
        }

        return true;
    }
}