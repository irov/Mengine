#include "MockupRenderProgramVariable.h"

#include "Config/Algorithm.h"

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
        static void makeVariableData( MockupRenderProgramVariable::ProgramVariableDesc * const _variable, Vector<T> & _container, const T * _values, uint32_t _size, uint32_t _count )
        {
            _variable->offset = (uint32_t)_container.size();
            _variable->size = _size;
            _variable->count = _count;

            _container.insert( _container.end(), _values, _values + _size * _count );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgramVariable::setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( &v, m_dataFloats, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgramVariable::setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        ProgramVariableDesc v;
        Detail::makeVariableData( &v, m_pixelFloats, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderProgramVariable::updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        const ProgramVariableDesc & v = m_pixelVariables[_index];

        float * values = &m_dataFloats[v.offset];
        Algorithm::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
}