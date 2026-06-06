#pragma once

#include "DX9RenderProgramVariableInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderProgramVariableStatic
        : public DX9RenderProgramVariableInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX9RenderProgramVariableStatic );

    public:
        DX9RenderProgramVariableStatic();
        ~DX9RenderProgramVariableStatic() override;

    public:
        bool initialize( uint32_t _vertexCount, uint32_t _pixelCount ) override;
        void finalize() override;

    public:
        void setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;
        void setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;

    public:
        void updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;

    public:
        bool apply( IDirect3DDevice9 * _pD3DDevice, const RenderProgramInterfacePtr & _program ) override;

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
    typedef IntrusivePtr<DX9RenderProgramVariableStatic, RenderProgramVariableInterface> DX9RenderProgramVariableStaticPtr;
    //////////////////////////////////////////////////////////////////////////
}
