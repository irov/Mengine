#pragma once

#include "Interface/RenderProgramVariableInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderProgramVariable
        : public RenderProgramVariableInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MockupRenderProgramVariable );

    public:
        MockupRenderProgramVariable();
        ~MockupRenderProgramVariable() override;

    public:
        bool initialize( uint32_t _vertexCount, uint32_t _pixelCount );
        void finalize();

    public:
        void setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;
        void setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;

    public:
        void updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;

    public:
        struct ProgramVariableDesc
        {
            uint32_t offset;
            uint32_t size;
            uint32_t count;
        };

    protected:
        typedef Vector<float> VectorDataFloats;
        VectorDataFloats m_dataFloats;
        VectorDataFloats m_pixelFloats;

        typedef Vector<ProgramVariableDesc> VectorVariables;
        VectorVariables m_vertexVariables;
        VectorVariables m_pixelVariables;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderProgramVariable, RenderProgramVariableInterface> MockupRenderProgramVariablePtr;
    //////////////////////////////////////////////////////////////////////////
}