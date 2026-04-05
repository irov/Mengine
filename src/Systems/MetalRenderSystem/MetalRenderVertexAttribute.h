#pragma once

#include "Interface/RenderSystemInterface.h"

#include "MetalRenderResourceHandler.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderVertexAttribute
        : public RenderVertexAttributeInterface
        , public MetalRenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderVertexAttribute );

    public:
        MetalRenderVertexAttribute();
        ~MetalRenderVertexAttribute() override;

    public:
        bool initialize( const ConstString & _name, uint32_t _elementSize );
        void finalize();

    protected:
        const ConstString & getName() const override;
        uint32_t getElementSize() const override;

    protected:
        void addAttribute( const ConstString & _uniform, uint32_t _index, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset ) override;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    public:
        struct AttributeDesc
        {
            ConstString uniform;
            uint32_t index;
            uint32_t size;
            EVertexAttributeType type;
            uint32_t stride;
            uint32_t offset;
            bool normalized;
        };

        typedef Vector<AttributeDesc> VectorAttribute;

        const VectorAttribute & getAttributes() const;

    protected:
        ConstString m_name;
        uint32_t m_elementSize;

        VectorAttribute m_attributes;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderVertexAttribute, RenderVertexAttributeInterface> MetalRenderVertexAttributePtr;
    //////////////////////////////////////////////////////////////////////////
}
