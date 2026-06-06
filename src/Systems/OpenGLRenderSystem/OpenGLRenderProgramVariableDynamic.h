#pragma once

#include "OpenGLRenderProgramVariableInterface.h"

#include "OpenGLRenderProgram.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderProgramVariableDynamic
        : public OpenGLRenderProgramVariableInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( OpenGLRenderProgramVariableDynamic );

    public:
        OpenGLRenderProgramVariableDynamic();
        ~OpenGLRenderProgramVariableDynamic() override;

    public:
        bool initialize( uint32_t _vertexCount, uint32_t _pixelCount ) override;
        void finalize() override;

    public:
        void setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;
        void setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;

    public:
        void updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;

    public:
        bool apply( const RenderProgramInterfacePtr & _program ) override;

    public:
        struct ProgramVariableDesc
        {
            Char uniform[32 + 1] = {'\0'};
            Vector<float> data;
            uint32_t size = 0;
            uint32_t count = 0;
            mutable GLint location = -1;
        };

    protected:
        typedef Vector<ProgramVariableDesc> VectorVariables;
        VectorVariables m_vertexVariables;
        VectorVariables m_pixelVariables;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderProgramVariableDynamic, RenderProgramVariableInterface> OpenGLRenderProgramVariableDynamicPtr;
    //////////////////////////////////////////////////////////////////////////
}
