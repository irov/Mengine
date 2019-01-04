#pragma once

#include "Interface/RenderProgramInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderProgramVariableInterface
        : public Mixin
    {
    public:
        virtual void setVertexVariableFloats( const Char * _uniform, uint32_t _index, float * _values, uint32_t _size, uint32_t _count ) = 0;
        virtual void setVertexVariableIntegers( const Char * _uniform, uint32_t _index, int32_t * _values, uint32_t _size, uint32_t _count ) = 0;
        virtual void setVertexVariableBooleans( const Char * _uniform, uint32_t _index, int32_t * _values, uint32_t _size, uint32_t _count ) = 0;

    public:
        virtual void setPixelVariableFloats( const Char * _uniform, uint32_t _index, float * _values, uint32_t _size, uint32_t _count ) = 0;
        virtual void setPixelVariableIntegers( const Char * _uniform, uint32_t _index, int32_t * _values, uint32_t _size, uint32_t _count ) = 0;
        virtual void setPixelVariableBooleans( const Char * _uniform, uint32_t _index, int32_t * _values, uint32_t _size, uint32_t _count ) = 0;

    public:
        virtual void updatePixelVariableFloats( uint32_t _index, float * _values, uint32_t _size, uint32_t _count ) = 0;
        virtual void updatePixelVariableIntegers( uint32_t _index, int32_t * _values, uint32_t _size, uint32_t _count ) = 0;
        virtual void updatePixelVariableBooleans( uint32_t _index, int32_t * _values, uint32_t _size, uint32_t _count ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderProgramVariableInterface> RenderProgramVariableInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
