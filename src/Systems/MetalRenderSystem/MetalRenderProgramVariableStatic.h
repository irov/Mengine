#pragma once

#include "MetalRenderProgramVariableInterface.h"

#include "MetalRenderProgram.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderProgramVariableStatic
        : public MetalRenderProgramVariableInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderProgramVariableStatic );

    public:
        MetalRenderProgramVariableStatic();
        ~MetalRenderProgramVariableStatic() override;

    public:
        bool initialize( uint32_t _vertexCount, uint32_t _pixelCount ) override;
        void finalize() override;

    public:
        void setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;
        void setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;

    public:
        void updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) override;

    public:
        bool apply( id<MTLRenderCommandEncoder> _encoder ) override;

    public:
        struct ProgramVariableDesc
        {
            Char uniform[32 + 1] = {'\0'};
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
    typedef IntrusivePtr<MetalRenderProgramVariableStatic, RenderProgramVariableInterface> MetalRenderProgramVariableStaticPtr;
    //////////////////////////////////////////////////////////////////////////
}
