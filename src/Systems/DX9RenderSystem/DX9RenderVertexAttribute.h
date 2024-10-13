#pragma once

#include "Interface/RenderVertexAttributeInterface.h"
#include "Interface/MemoryInterface.h"

#include "Environment/DirectX9/DX9RenderIncluder.h"

#include "Kernel/ReferenceCounter.h"
#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderVertexAttribute
        : public RenderVertexAttributeInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX9RenderVertexAttribute );

    public:
        DX9RenderVertexAttribute();
        ~DX9RenderVertexAttribute() override;

    public:
        bool initialize( const ConstString & _name, uint32_t _elementSize );
        void finalize();

    public:
        bool compile( IDirect3DDevice9 * _pD3DDevice );
        void release();

    public:
        void enable( IDirect3DDevice9 * _pD3DDevice );
        void disable( IDirect3DDevice9 * _pD3DDevice );

    public:
        const ConstString & getName() const override;
        uint32_t getElementSize() const override;

    public:
        void addAttribute( const ConstString & _uniform, uint32_t _index, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset ) override;

    protected:
        ConstString m_name;
        uint32_t m_elementSize;

        ReferenceCounter m_compileReferenceCount;

        IDirect3DVertexDeclaration9 * m_pD3DVertexDeclaration;

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
    typedef IntrusivePtr<DX9RenderVertexAttribute, RenderVertexAttributeInterface> DX9RenderVertexAttributePtr;
    //////////////////////////////////////////////////////////////////////////
}