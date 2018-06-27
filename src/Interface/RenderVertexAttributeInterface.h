#pragma once

#include "Interface/ServantInterface.h"

#include "Interface/RenderEnumInterface.h"

#include "Core/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderVertexAttributeInterface
        : public ServantInterface
    {
    public:
        virtual const ConstString & getName() const = 0;
        virtual uint32_t getElementSize() const = 0;

    public:
        virtual void addAttribute( const ConstString & _uniform, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset ) = 0;

    public:
        virtual bool enable() = 0;
        virtual void disable() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderVertexAttributeInterface> RenderVertexAttributeInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
