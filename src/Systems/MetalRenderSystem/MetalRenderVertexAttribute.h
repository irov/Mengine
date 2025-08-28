#pragma once

#include "Interface/RenderVertexAttributeInterface.h"
#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class MetalRenderVertexAttribute
        : public RenderVertexAttributeInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderVertexAttribute );

    public:
        MetalRenderVertexAttribute();
        ~MetalRenderVertexAttribute() override;

    public:
        bool initialize( const ConstString & _name, uint32_t _elementSize );
        void finalize();

    public:
        const ConstString & getName() const override;
        uint32_t getElementSize() const override;

    public:
        void addAttribute( const ConstString & _uniform, uint32_t _index, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset ) override;

    protected:
        struct Attribute
        {
            ConstString uniform;
            uint32_t index;
            uint32_t size;
            EVertexAttributeType type;
            bool normalized;
            uint32_t stride;
            uint32_t offset;
        };

        ConstString m_name;
        uint32_t m_elementSize;
        Vector<Attribute> m_attributes;
    };

    typedef IntrusivePtr<MetalRenderVertexAttribute, RenderVertexAttributeInterface> MetalRenderVertexAttributePtr;
}

