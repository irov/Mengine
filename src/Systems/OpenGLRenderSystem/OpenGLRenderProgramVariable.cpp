#include "OpenGLRenderProgramVariable.h"

#include "OpenGLRenderErrorHelper.h"
#include "OpenGLRenderEnum.h"
#include "OpenGLRenderProgram.h"

#include "Kernel/Logger.h"

#include "Config/StdString.h"

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
        static void makeVariableData( OpenGLRenderProgramVariable::ProgramVariableDesc & _variable, Vector<T> & _container, const Char * _uniform, OpenGLRenderProgramVariable::EProgramVariableType _type, const T * _values, uint32_t _size, uint32_t _count )
        {
            MENGINE_STRCPY( _variable.uniform, _uniform );
            _variable.type = _type;
            _variable.offset = (uint32_t)_container.size();
            _variable.size = _size;
            _variable.count = _count;

            _container.insert( _container.end(), _values, _values + _size * _count );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableFloats( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataFloats, _uniform, EPVT_FLOAT, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableIntegers( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataIntegers, _uniform, EPVT_INTEGER, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariableBooleans( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataBooleans, _uniform, EPVT_BOOLEAN, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableFloats( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_pixelFloats, _uniform, EPVT_FLOAT, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableIntegers( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_pixelIntegers, _uniform, EPVT_INTEGER, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariableBooleans( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_pixelBooleans, _uniform, EPVT_BOOLEAN, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::updatePixelVariableFloats( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc & v = m_pixelVariables[_index];

        float * values = m_pixelFloats.data() + v.offset;
        std::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::updatePixelVariableIntegers( uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc & v = m_pixelVariables[_index];

        int32_t * values = m_pixelIntegers.data() + v.offset;
        std::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::updatePixelVariableBooleans( uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc & v = m_pixelVariables[_index];

        int32_t * values = m_pixelBooleans.data() + v.offset;
        std::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgramVariable::apply( const RenderProgramInterfacePtr & _program )
    {
        OpenGLRenderProgram * openglProgram = stdex::intrusive_get<OpenGLRenderProgram *>( _program );

        GLuint programId = openglProgram->getProgramId();

        uint32_t vertexEnumerator = 0;
        for( ProgramVariableDesc & v : m_vertexVariables )
        {
            if( v.location == -1 )
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

                v.location = location;
            }

            switch( v.type )
            {
            case EPVT_FLOAT:
                {
                    const float * buff = m_dataFloats.data() + v.offset;

                    switch( v.size )
                    {
                    case 1:
                        {
                            GLCALL( glUniform1fv, (v.location, v.count, buff) );
                        }break;
                    case 2:
                        {
                            GLCALL( glUniform2fv, (v.location, v.count, buff) );
                        }break;
                    case 3:
                        {
                            GLCALL( glUniform3fv, (v.location, v.count, buff) );
                        }break;
                    case 4:
                        {
                            GLCALL( glUniform4fv, (v.location, v.count, buff) );
                        }break;
                    default:
                        return false;
                    }
                }break;
            case EPVT_INTEGER:
                {
                    const int32_t * buff = m_dataIntegers.data() + v.offset;

                    switch( v.size )
                    {
                    case 1:
                        {
                            GLCALL( glUniform1iv, (v.location, v.count, buff) );
                        }break;
                    case 2:
                        {
                            GLCALL( glUniform2iv, (v.location, v.count, buff) );
                        }break;
                    case 3:
                        {
                            GLCALL( glUniform3iv, (v.location, v.count, buff) );
                        }break;
                    case 4:
                        {
                            GLCALL( glUniform4iv, (v.location, v.count, buff) );
                        }break;
                    default:
                        return false;
                    }
                }break;
            case EPVT_BOOLEAN:
                {
                    const int32_t * buff = m_dataBooleans.data() + v.offset;

                    switch( v.size )
                    {
                    case 1:
                        {
                            GLCALL( glUniform1iv, (v.location, v.count, buff) );
                        }break;
                    case 2:
                        {
                            GLCALL( glUniform2iv, (v.location, v.count, buff) );
                        }break;
                    case 3:
                        {
                            GLCALL( glUniform3iv, (v.location, v.count, buff) );
                        }break;
                    case 4:
                        {
                            GLCALL( glUniform4iv, (v.location, v.count, buff) );
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
        for( ProgramVariableDesc & v : m_pixelVariables )
        {
            if( v.location == -1 )
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

                v.location = location;
            }

            switch( v.type )
            {
            case EPVT_FLOAT:
                {
                    const float * buff = m_pixelFloats.data() + v.offset;

                    switch( v.size )
                    {
                    case 1:
                        {
                            GLCALL( glUniform1fv, (v.location, v.count, buff) );
                        }break;
                    case 2:
                        {
                            GLCALL( glUniform2fv, (v.location, v.count, buff) );
                        }break;
                    case 3:
                        {
                            GLCALL( glUniform3fv, (v.location, v.count, buff) );
                        }break;
                    case 4:
                        {
                            GLCALL( glUniform4fv, (v.location, v.count, buff) );
                        }break;
                    default:
                        return false;
                    }
                }break;
            case EPVT_INTEGER:
                {
                    const int32_t * buff = m_pixelIntegers.data() + v.offset;

                    switch( v.size )
                    {
                    case 1:
                        {
                            GLCALL( glUniform1iv, (v.location, v.count, buff) );
                        }break;
                    case 2:
                        {
                            GLCALL( glUniform2iv, (v.location, v.count, buff) );
                        }break;
                    case 3:
                        {
                            GLCALL( glUniform3iv, (v.location, v.count, buff) );
                        }break;
                    case 4:
                        {
                            GLCALL( glUniform4iv, (v.location, v.count, buff) );
                        }break;
                    default:
                        return false;
                    }
                }break;
            case EPVT_BOOLEAN:
                {
                    const int32_t * buff = m_pixelBooleans.data() + v.offset;

                    switch( v.size )
                    {
                    case 1:
                        {
                            GLCALL( glUniform1iv, (v.location, v.count, buff) );
                        }break;
                    case 2:
                        {
                            GLCALL( glUniform2iv, (v.location, v.count, buff) );
                        }break;
                    case 3:
                        {
                            GLCALL( glUniform3iv, (v.location, v.count, buff) );
                        }break;
                    case 4:
                        {
                            GLCALL( glUniform4iv, (v.location, v.count, buff) );
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
    //////////////////////////////////////////////////////////////////////////
}
