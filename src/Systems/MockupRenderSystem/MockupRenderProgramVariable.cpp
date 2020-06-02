#include "MockupRenderProgramVariable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderProgramVariable::MockupRenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderProgramVariable::~MockupRenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderProgramVariable::initialize( uint32_t _vertexCount, uint32_t _pixelCount )
    {
        m_vertexVariables.resize( _vertexCount );
        m_pixelVariables.resize( _pixelCount );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgramVariable::finalize()
    {
        m_vertexVariables.clear();
        m_pixelVariables.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class T>
        static void makeVariableData( ProgramVariableDesc & _variable, Vector<T> & _container, EProgramVariableType _type, const T * _values, uint32_t _size, uint32_t _count )
        {
            _variable.type = _type;
            _variable.offset = (uint32_t)_container.size();
            _variable.size = _size;
            _variable.count = _count;

            _container.insert( _container.end(), _values, _values + _size * _count );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgramVariable::setVertexVariableFloats( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataFloats, EPVT_FLOAT, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgramVariable::setVertexVariableIntegers( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataIntegers, EPVT_INTEGER, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgramVariable::setVertexVariableBooleans( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataBooleans, EPVT_BOOLEAN, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgramVariable::setPixelVariableFloats( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_pixelFloats, EPVT_FLOAT, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgramVariable::setPixelVariableIntegers( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_pixelIntegers, EPVT_INTEGER, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgramVariable::setPixelVariableBooleans( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_pixelBooleans, EPVT_BOOLEAN, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgramVariable::updatePixelVariableFloats( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        const ProgramVariableDesc & v = m_pixelVariables[_index];

        float * values = &m_dataFloats[v.offset];
        std::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgramVariable::updatePixelVariableIntegers( uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        const ProgramVariableDesc & v = m_pixelVariables[_index];

        int32_t * values = &m_dataIntegers[v.offset];
        std::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgramVariable::updatePixelVariableBooleans( uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        const ProgramVariableDesc & v = m_pixelVariables[_index];

        int32_t * values = &m_dataBooleans[v.offset];
        std::copy( _values, _values + _size * _count, values );
    }
}