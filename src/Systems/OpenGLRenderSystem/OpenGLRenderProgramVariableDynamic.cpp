#include "OpenGLRenderProgramVariableDynamic.h"

#include "OpenGLRenderErrorHelper.h"
#include "OpenGLRenderEnum.h"
#include "OpenGLRenderProgram.h"

#include "Kernel/Logger.h"

#include "Config/StdString.h"
#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        static bool applyVariable( OpenGLRenderProgram * _program, GLuint _programId, const OpenGLRenderProgramVariableDynamic::ProgramVariableDesc & _variable )
        {
            if( _variable.count == 0 )
            {
                return true;
            }

            if( _variable.location == -1 )
            {
                GLint location;
                MENGINE_GLCALLR( location, glGetUniformLocation, (_programId, _variable.uniform) );

                MENGINE_ASSERTION_FATAL( location != -1, "program '%s' not found uniform sampler '%s'"
                    , _program->getName().c_str()
                    , _variable.uniform
                );

                _variable.location = location;
            }

            const float * buff = _variable.data.data();

            switch( _variable.size )
            {
            case 1:
                {
                    MENGINE_IF_GLCALL( glUniform1fv, (_variable.location, _variable.count, buff) )
                    {
                        return false;
                    }
                }break;
            case 2:
                {
                    MENGINE_IF_GLCALL( glUniform2fv, (_variable.location, _variable.count, buff) )
                    {
                        return false;
                    }
                }break;
            case 3:
                {
                    MENGINE_IF_GLCALL( glUniform3fv, (_variable.location, _variable.count, buff) )
                    {
                        return false;
                    }
                }break;
            case 4:
                {
                    MENGINE_IF_GLCALL( glUniform4fv, (_variable.location, _variable.count, buff) )
                    {
                        return false;
                    }
                }break;
            default:
                return false;
            }

            return true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderProgramVariableDynamic::OpenGLRenderProgramVariableDynamic()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderProgramVariableDynamic::~OpenGLRenderProgramVariableDynamic()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgramVariableDynamic::initialize( uint32_t _vertexCount, uint32_t _pixelCount )
    {
        m_vertexVariables.resize( _vertexCount );
        m_pixelVariables.resize( _pixelCount );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariableDynamic::finalize()
    {
        m_vertexVariables.clear();
        m_pixelVariables.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariableDynamic::setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        if( _index >= (uint32_t)m_vertexVariables.size() )
        {
            m_vertexVariables.resize( _index + 1 );
        }

        ProgramVariableDesc & v = m_vertexVariables[_index];
        StdString::strcpy_safe( v.uniform, _uniform, 32 );
        v.size = _size;
        v.count = _count;
        v.location = -1;
        v.data.assign( _values, _values + _size * _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariableDynamic::setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        if( _index >= (uint32_t)m_pixelVariables.size() )
        {
            m_pixelVariables.resize( _index + 1 );
        }

        ProgramVariableDesc & v = m_pixelVariables[_index];
        StdString::strcpy_safe( v.uniform, _uniform, 32 );
        v.size = _size;
        v.count = _count;
        v.location = -1;
        v.data.assign( _values, _values + _size * _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgramVariableDynamic::updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        if( _index >= (uint32_t)m_pixelVariables.size() )
        {
            return;
        }

        ProgramVariableDesc & v = m_pixelVariables[_index];

        MENGINE_UNUSED( _size );
        MENGINE_UNUSED( _count );

        StdAlgorithm::copy( _values, _values + v.size * v.count, v.data.data() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgramVariableDynamic::apply( const RenderProgramInterfacePtr & _program )
    {
        OpenGLRenderProgram * openglProgram = stdex::intrusive_get<OpenGLRenderProgram *>( _program );

        GLuint programId = openglProgram->getProgramId();

        for( const ProgramVariableDesc & v : m_vertexVariables )
        {
            if( Detail::applyVariable( openglProgram, programId, v ) == false )
            {
                return false;
            }
        }

        for( const ProgramVariableDesc & v : m_pixelVariables )
        {
            if( Detail::applyVariable( openglProgram, programId, v ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
