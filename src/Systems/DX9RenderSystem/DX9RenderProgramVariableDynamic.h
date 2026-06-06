#pragma once

#include "DX9RenderProgramVariableInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderProgramVariableDynamic
        : public DX9RenderProgramVariableInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX9RenderProgramVariableDynamic );

    public:
        DX9RenderProgramVariableDynamic();
        ~DX9RenderProgramVariableDynamic() override;

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
            Vector<float> data;
            uint32_t size = 0;
            uint32_t count = 0;
            uint32_t regIndex = 0;
        };

    protected:
        typedef Vector<ProgramVariableDesc> VectorVariables;
        VectorVariables m_vertexVariables;
        VectorVariables m_pixelVariables;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderProgramVariableDynamic, RenderProgramVariableInterface> DX9RenderProgramVariableDynamicPtr;
    //////////////////////////////////////////////////////////////////////////
}
