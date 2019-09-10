#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"

#include "Config/Vector.h"

#include "d3d9.h"

namespace Mengine
{
    class DX9RenderVertexAttribute
        : public RenderVertexAttributeInterface
        , public Factorable
    {
    public:
        DX9RenderVertexAttribute();
        ~DX9RenderVertexAttribute() override;

    public:
        bool initialize( const ConstString & _name, uint32_t _elementSize );
        void finalize();

    public:
        bool compile( IDirect3DDevice9 * _pD3DDevice );

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

        IDirect3DDevice9 * m_pD3DDevice;
        IDirect3DVertexDeclaration9 * m_vertexDeclaration;

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
    typedef IntrusivePtr<DX9RenderVertexAttribute> DX9RenderVertexAttributePtr;
}