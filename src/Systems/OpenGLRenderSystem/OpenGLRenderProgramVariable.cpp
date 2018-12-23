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
        m_vertexVariables.clear();
        m_pixelVariables.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class T>
        void makeVariableData( OpenGLRenderProgramVariable::Variable & _variable, Vector<T> & _container, const Char * _uniform, uint32_t _type, T * _values, uint32_t _count )
        {
            strcpy( _variable.uniform, _uniform );
            _variable.type = _type;
            _variable.offset = _container.size();
            _variable.count = _count;

            _container.insert( _container.end(), _values, _values + _count );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableFloats( const Char * _uniform, uint32_t _index, float * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataFloats, _uniform, 0, _values, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableIntegers( const Char * _uniform, uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataIntegers, _uniform, 1, _values, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableBooleans( const Char * _uniform, uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataBooleans, _uniform, 2, _values, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableFloats( const Char * _uniform, uint32_t _index, float * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataFloats, _uniform, 0, _values, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableIntegers( const Char * _uniform, uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataIntegers, _uniform, 1, _values, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableBooleans( const Char * _uniform, uint32_t _index, int32_t * _values, uint32_t _count )
    {
        Variable v;
        Detail::makeVariableData( v, m_dataBooleans, _uniform, 2, _values, _count );

        m_pixelVariables[_index] = v;
    }
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderProgramVariable::updatePixelVariableFloats( uint32_t _index, float * _values, uint32_t _count )
	{
        Variable & v = m_pixelVariables[_index];

        float * values = &m_dataFloats[v.offset];
        std::copy( _values, _values + _count, values );
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderProgramVariable::updatePixelVariableIntegers( uint32_t _index, int32_t * _values, uint32_t _count )
	{
        Variable & v = m_pixelVariables[_index];

        int32_t * values = &m_dataIntegers[v.offset];
        std::copy( _values, _values + _count, values );
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderProgramVariable::updatePixelVariableBooleans( uint32_t _index, int32_t * _values, uint32_t _count )
	{
        Variable & v = m_pixelVariables[_index];

        int32_t * values = &m_dataBooleans[v.offset];
        std::copy( _values, _values + _count, values );
	}
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgramVariable::apply( const RenderProgramInterfacePtr & _program )
    {
        (void)_program;

		return false;
    }
}