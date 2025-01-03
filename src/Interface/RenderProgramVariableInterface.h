#pragma once

#include "Kernel/Mixin.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderProgramVariableInterface
        : public Mixin
    {
    public:
        virtual void setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) = 0;
        virtual void setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) = 0;

    public:
        virtual void updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderProgramVariableInterface> RenderProgramVariableInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
