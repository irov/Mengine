#include "OpenGLRenderProgramVariable.h"

#include "OpenGLRenderErrorHelper.h"
#include "OpenGLRenderEnum.h"
#include "OpenGLRenderProgram.h"

#include "Kernel/Logger.h"

#include "Config/StdString.h"
#include "Config/Algorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        template<class T>
        static void makeVariableData( OpenGLRenderProgramVariable::ProgramVariableDesc & _variable, Vector<T> & _container, const Char * _uniform, const T * _values, uint32_t _size, uint32_t _count )
        {
            StdString::strcpy( _variable.uniform, _uniform );
            _variable.offset = (uint32_t)_container.size();
            _variable.size = _size;
            _variable.count = _count;

            _container.insert( _container.end(), _values, _values + _size * _count );
        }
    }
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
        m_dataFloats.clear();
        m_pixelFloats.clear();

        m_vertexVariables.clear();
        m_pixelVariables.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_dataFloats, _uniform, _values, _size, _count );

        m_vertexVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc v;
        Detail::makeVariableData( v, m_pixelFloats, _uniform, _values, _size, _count );

        m_pixelVariables[_index] = v;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariable::updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        ProgramVariableDesc & v = m_pixelVariables[_index];

        float * values = m_pixelFloats.data() + v.offset;
        Algorithm::copy( _values, _values + _size * _count, values );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgramVariable::apply( const RenderProgramInterfacePtr & _program )
    {
        OpenGLRenderProgram * openglProgram = stdex::intrusive_get<OpenGLRenderProgram *>( _program );

        GLuint programId = openglProgram->getProgramId();

        uint32_t vertexEnumerator = 0;
        for( const ProgramVariableDesc & v : m_vertexVariables )
        {
            if( v.location == -1 )
            {
                GLint location;
                MENGINE_GLCALLR( location, glGetUniformLocation, (programId, v.uniform) );

                if( location == -1 )
                {
                    LOGGER_ASSERTION( "program '%s' not found uniform sampler '%s'"
                        , openglProgram->getName().c_str()
                        , v.uniform
                    );

                    return false;
                }

                v.location = location;
            }

            const float * buff = m_dataFloats.data() + v.offset;

            switch( v.size )
            {
            case 1:
                {
                    MENGINE_IF_GLCALL( glUniform1fv, (v.location, v.count, buff) )
                    {
                        return false;
                    }
                }break;
            case 2:
                {
                    MENGINE_IF_GLCALL( glUniform2fv, (v.location, v.count, buff) )
                    {
                        return false;
                    }
                }break;
            case 3:
                {
                    MENGINE_IF_GLCALL( glUniform3fv, (v.location, v.count, buff) )
                    {
                        return false;
                    }
                }break;
            case 4:
                {
                    MENGINE_IF_GLCALL( glUniform4fv, (v.location, v.count, buff) )
                    {
                        return false;
                    }
                }break;
            default:
                return false;
            }

            ++vertexEnumerator;
        }

        uint32_t pixelEnumerator = 0;
        for( const ProgramVariableDesc & v : m_pixelVariables )
        {
            if( v.location == -1 )
            {
                GLint location;
                MENGINE_GLCALLR( location, glGetUniformLocation, (programId, v.uniform) );

                if( location == -1 )
                {
                    LOGGER_ASSERTION( "program '%s' not found uniform sampler '%s'"
                        , openglProgram->getName().c_str()
                        , v.uniform
                    );

                    return false;
                }

                v.location = location;
            }

            const float * buff = m_pixelFloats.data() + v.offset;

            switch( v.size )
            {
            case 1:
                {
                    MENGINE_IF_GLCALL( glUniform1fv, (v.location, v.count, buff) )
                    {
                        return false;
                    }
                }break;
            case 2:
                {
                    MENGINE_IF_GLCALL( glUniform2fv, (v.location, v.count, buff) )
                    {
                        return false;
                    }
                }break;
            case 3:
                {
                    MENGINE_IF_GLCALL( glUniform3fv, (v.location, v.count, buff) )
                    {
                        return false;
                    }
                }break;
            case 4:
                {
                    MENGINE_IF_GLCALL( glUniform4fv, (v.location, v.count, buff) )
                    {
                        return false;
                    }
                }break;
            default:
                return false;
            }

            ++pixelEnumerator;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
