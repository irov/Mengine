#pragma once

#include "Interface/RenderVertexAttributeInterface.h"
#include "Interface/MemoryInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderVertexAttribute
        : public RenderVertexAttributeInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX11RenderVertexAttribute );

    public:
        DX11RenderVertexAttribute();
        ~DX11RenderVertexAttribute() override;

    public:
        bool initialize( const ConstString & _name, uint32_t _elementSize );
        void finalize();

    public:
        bool compile( const ID3D11DevicePtr & _pD3DDevice, const MemoryInterfacePtr & _shaderCompileMemory );
        void release();

    public:
        void enable( const ID3D11DeviceContextPtr & _pD3DDeviceContext );
        void disable( const ID3D11DeviceContextPtr & _pD3DDeviceContext );

    public:
        const ConstString & getName() const override;
        uint32_t getElementSize() const override;

    public:
        void addAttribute( const ConstString & _uniform, uint32_t _index, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset ) override;

    protected:
        ConstString m_name;
        uint32_t m_elementSize;

        uint32_t m_compileReferenceCount;

        ID3D11InputLayoutPtr m_pD3DVertexDeclaration;

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
        VectorAttribute m_attributes;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderVertexAttribute, RenderVertexAttributeInterface> DX11RenderVertexAttributePtr;
    //////////////////////////////////////////////////////////////////////////
}