#pragma once

#include "MetalRenderProgramVariableInterface.h"

#include "MetalRenderProgram.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderProgramVariableDynamic
        : public MetalRenderProgramVariableInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderProgramVariableDynamic );

    public:
        MetalRenderProgramVariableDynamic();
        ~MetalRenderProgramVariableDynamic() override;

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
            Vector<float> data;
            uint32_t size = 0;
            uint32_t count = 0;
        };

    protected:
        typedef Vector<ProgramVariableDesc> VectorVariables;
        VectorVariables m_vertexVariables;
        VectorVariables m_pixelVariables;

        typedef Vector<float> VectorDataFloats;
        VectorDataFloats m_dataFloats;
        VectorDataFloats m_pixelFloats;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderProgramVariableDynamic, RenderProgramVariableInterface> MetalRenderProgramVariableDynamicPtr;
    //////////////////////////////////////////////////////////////////////////
}
