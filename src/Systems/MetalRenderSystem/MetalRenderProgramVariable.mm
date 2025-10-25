#include "MetalRenderProgramVariable.h"

#include "Config/StdString.h"
#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class T>
        static void makeVariableData( MetalRenderProgramVariable::ProgramVariableDesc & _variable, Vector<T> & _container, const Char * _uniform, const T * _values, uint32_t _size, uint32_t _count )
        {
            StdString::strcpy_safe( _variable.uniform, _uniform, 32 );

            _variable.offset = (uint32_t)_container.size();
            _variable.size = _size;
            _variable.count = _count;

            _container.insert( _container.end(), _values, _values + _size * _count );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderProgramVariable::MetalRenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderProgramVariable::~MetalRenderProgramVariable() = default;
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderProgramVariable::initialize( uint32_t _vertexCount, uint32_t _pixelCount )
    {
        m_vertexVariables.resize( _vertexCount );
        m_pixelVariables.resize( _pixelCount );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgramVariable::finalize()
    {
        m_dataFloats.clear();
        m_pixelFloats.clear();

        m_vertexVariables.clear();
        m_pixelVariables.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgramVariable::setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataFloats, _uniform, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgramVariable::setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_pixelFloats, _uniform, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgramVariable::updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc & v = m_pixelVariables[_index];

        float * dest = m_pixelFloats.data() + v.offset;

        StdAlgorithm::copy( _values, _values + _size * _count, dest );
    }
    //////////////////////////////////////////////////////////////////////////
}

