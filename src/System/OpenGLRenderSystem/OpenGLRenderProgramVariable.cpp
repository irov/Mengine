#include "OpenGLRenderProgramVariable.h"

#include "OpenGLRenderError.h"
#include "OpenGLRenderEnum.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderProgramVariable::OpenGLRenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderProgramVariable::~OpenGLRenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgramVariable::initialize( uint32_t _vertexCount, uint32_t _pixelCount )
    {
        m_vertexVariables.resize( _vertexCount );
        m_pixelVariables.resize( _pixelCount );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class T>
        void makeVariableData( OpenGLRenderProgramVariable::Variable & _variable, Vector<T> & _container, uint32_t _type, T * _values, uint32_t _count )
        {
            _variable.type = _type;
            _variable.offset = _container.size();
            _variable.count = _count;

            _container.insert( _container.end(), _values, _values + _count );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableFloats( uint32_t _index, float * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataFloats, 0, _values, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableIntegers( uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataIntegers, 1, _values, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableBooleans( uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataBooleans, 2, _values, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableFloats( uint32_t _index, float * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataFloats, 0, _values, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableIntegers( uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataIntegers, 1, _values, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableBooleans( uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataBooleans, 2, _values, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgramVariable::apply( const RenderProgramInterfacePtr & _program )
    {
        (void)_program;

		return false;
    }
}