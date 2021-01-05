#pragma once

#include "Interface/RenderVertexAttributeInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderVertexAttribute
        : public RenderVertexAttributeInterface
        , public Factorable
    {
    public:
        MockupRenderVertexAttribute();
        ~MockupRenderVertexAttribute() override;

    public:
        bool initialize( const ConstString & _name, uint32_t _elementSize );
        void finalize();

    public:
        void enable() override;
        void disable() override;

    public:
        const ConstString & getName() const override;
        uint32_t getElementSize() const override;

    public:
        void addAttribute( const ConstString & _uniform, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset ) override;

    protected:
        ConstString m_name;
        uint32_t m_elementSize;

        struct AttributeDesc
        {
            ConstString uniform;

            uint32_t size;
            EVertexAttributeType type;
            uint32_t stride;
            uint32_t offset;

            bool normalized;
        };

        typedef Vector<AttributeDesc> VectorAttribute;
        VectorAttribute m_attributes;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderVertexAttribute, RenderVertexAttributeInterface> MockupRenderVertexAttributePtr;
    //////////////////////////////////////////////////////////////////////////
}