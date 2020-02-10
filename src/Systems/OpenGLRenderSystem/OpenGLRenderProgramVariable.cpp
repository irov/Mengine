#include "OpenGLRenderProgramVariable.h"

#include "OpenGLRenderError.h"
#include "OpenGLRenderEnum.h"
#include "OpenGLRenderProgram.h"

#include "Kernel/Logger.h"

#include <string.h>

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
        void makeVariableData( ProgramVariableDesc & _variable, Vector<T> & _container, const Char * _uniform, uint32_t _type, T * _values, uint32_t _size, uint32_t _count )
        {
            ::strcpy( _variable.uniform, _uniform );
            _variable.type = _type;
            _variable.offset = (uint32_t)_container.size();
            _variable.size = _size;
            _variable.count = _count;

            _container.insert( _container.end(), _values, _values + _size * _count );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableFloats( const Char * _uniform, uint32_t _index, float * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataFloats, _uniform, 0, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableIntegers( const Char * _uniform, uint32_t _index, int32_t * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataIntegers, _uniform, 1, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableBooleans( const Char * _uniform, uint32_t _index, int32_t * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataBooleans, _uniform, 2, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableFloats( const Char * _uniform, uint32_t _index, float * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataFloats, _uniform, 0, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableIntegers( const Char * _uniform, uint32_t _index, int32_t * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataIntegers, _uniform, 1, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableBooleans( const Char * _uniform, uint32_t _index, int32_t * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataBooleans, _uniform, 2, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::updatePixelVariableFloats( uint32_t _index, float * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc & v = m_pixelVariables[_index];

        float * values = &m_dataFloats[v.offset];
        std::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::updatePixelVariableIntegers( uint32_t _index, int32_t * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc & v = m_pixelVariables[_index];

        int32_t * values = &m_dataIntegers[v.offset];
        std::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::updatePixelVariableBooleans( uint32_t _index, int32_t * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc & v = m_pixelVariables[_index];

        int32_t * values = &m_dataBooleans[v.offset];
        std::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgramVariable::apply( const RenderProgramInterfacePtr & _program )
    {
        OpenGLRenderProgram * openglProgram = stdex::intrusive_get<OpenGLRenderProgram *>( _program );

        GLuint programId = openglProgram->getProgramId();

        uint32_t vertexEnumerator = 0;
        for( const ProgramVariableDesc & v : m_vertexVariables )
        {
            GLint location;
            GLCALLR( location, glGetUniformLocation, (programId, v.uniform) );

            if( location == -1 )
            {
                LOGGER_ERROR( "program '%s' not found uniform sampler '%s'"
                    , openglProgram->getName().c_str()
                    , v.uniform
                );

                return false;
            }

            switch( v.type )
            {
            case 0:
                {
                    const float * buff = &m_dataFloats[v.offset];

                    switch( v.size )
                    {
                    case 1:
                        {
                            GLCALL( glUniform1fv, (location, v.count, buff) );
                        }break;
                    case 2:
                        {
                            GLCALL( glUniform2fv, (location, v.count, buff) );
                        }break;
                    case 3:
                        {
                            GLCALL( glUniform3fv, (location, v.count, buff) );
                        }break;
                    case 4:
                        {
                            GLCALL( glUniform4fv, (location, v.count, buff) );
                        }break;
                    default:
                        return false;
                    }
                }break;
            case 1:
                {
                    const int32_t * buff = &m_dataIntegers[v.offset];

                    switch( v.size )
                    {
                    case 1:
                        {
                            GLCALL( glUniform1iv, (location, v.count, buff) );
                        }break;
                    case 2:
                        {
                            GLCALL( glUniform2iv, (location, v.count, buff) );
                        }break;
                    case 3:
                        {
                            GLCALL( glUniform3iv, (location, v.count, buff) );
                        }break;
                    case 4:
                        {
                            GLCALL( glUniform4iv, (location, v.count, buff) );
                        }break;
                    default:
                        return false;
                    }
                }break;
            case 2:
                {
                    const int32_t * buff = &m_dataBooleans[v.offset];

                    switch( v.size )
                    {
                    case 1:
                        {
                            GLCALL( glUniform1iv, (location, v.count, buff) );
                        }break;
                    case 2:
                        {
                            GLCALL( glUniform2iv, (location, v.count, buff) );
                        }break;
                    case 3:
                        {
                            GLCALL( glUniform3iv, (location, v.count, buff) );
                        }break;
                    case 4:
                        {
                            GLCALL( glUniform4iv, (location, v.count, buff) );
                        }break;
                    default:
                        return false;
                    }
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
            GLint location;
            GLCALLR( location, glGetUniformLocation, (programId, v.uniform) );

            if( location == -1 )
            {
                LOGGER_ERROR( "program '%s' not found uniform sampler '%s'"
                    , openglProgram->getName().c_str()
                    , v.uniform
                );

                return false;
            }

            switch( v.type )
            {
            case 0:
                {
                    const float * buff = &m_dataFloats[v.offset];

                    switch( v.size )
                    {
                    case 1:
                        {
                            GLCALL( glUniform1fv, (location, v.count, buff) );
                        }break;
                    case 2:
                        {
                            GLCALL( glUniform2fv, (location, v.count, buff) );
                        }break;
                    case 3:
                        {
                            GLCALL( glUniform3fv, (location, v.count, buff) );
                        }break;
                    case 4:
                        {
                            GLCALL( glUniform4fv, (location, v.count, buff) );
                        }break;
                    default:
                        return false;
                    }
                }break;
            case 1:
                {
                    const int32_t * buff = &m_dataIntegers[v.offset];

                    switch( v.size )
                    {
                    case 1:
                        {
                            GLCALL( glUniform1iv, (location, v.count, buff) );
                        }break;
                    case 2:
                        {
                            GLCALL( glUniform2iv, (location, v.count, buff) );
                        }break;
                    case 3:
                        {
                            GLCALL( glUniform3iv, (location, v.count, buff) );
                        }break;
                    case 4:
                        {
                            GLCALL( glUniform4iv, (location, v.count, buff) );
                        }break;
                    default:
                        return false;
                    }
                }break;
            case 2:
                {
                    const int32_t * buff = &m_dataBooleans[v.offset];

                    switch( v.size )
                    {
                    case 1:
                        {
                            GLCALL( glUniform1iv, (location, v.count, buff) );
                        }break;
                    case 2:
                        {
                            GLCALL( glUniform2iv, (location, v.count, buff) );
                        }break;
                    case 3:
                        {
                            GLCALL( glUniform3iv, (location, v.count, buff) );
                        }break;
                    case 4:
                        {
                            GLCALL( glUniform4iv, (location, v.count, buff) );
                        }break;
                    default:
                        return false;
                    }
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
}
