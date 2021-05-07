#pragma once

#include "Interface/RenderEnumInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderVertexAttributeInterface
        : public Mixin
    {
    public:
        virtual const ConstString & getName() const = 0;
        virtual uint32_t getElementSize() const = 0;

    public:
        virtual void addAttribute( const ConstString & _uniform, uint32_t _index, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderVertexAttributeInterface> RenderVertexAttributeInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
