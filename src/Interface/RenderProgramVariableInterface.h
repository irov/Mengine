#pragma once

#include "Interface/ServantInterface.h"

#include "Interface/RenderProgramInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderProgramVariableInterface
        : public ServantInterface
    {
    public:
        virtual void setVertexVariableFloats( uint32_t _index, float * _values, uint32_t _count ) = 0;
        virtual void setVertexVariableIntegers( uint32_t _index, int32_t * _values, uint32_t _count ) = 0;
        virtual void setVertexVariableBooleans( uint32_t _index, int32_t * _values, uint32_t _count ) = 0;

    public:
        virtual void setPixelVariableFloats( uint32_t _index, float * _values, uint32_t _count ) = 0;
        virtual void setPixelVariableIntegers( uint32_t _index, int32_t * _values, uint32_t _count ) = 0;
        virtual void setPixelVariableBooleans( uint32_t _index, int32_t * _values, uint32_t _count ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderProgramVariableInterface> RenderProgramVariableInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
