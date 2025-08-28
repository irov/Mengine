#include "MetalRenderVertexAttribute.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderVertexAttribute::MetalRenderVertexAttribute()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderVertexAttribute::~MetalRenderVertexAttribute() = default;
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexAttribute::initialize( const ConstString & _name, uint32_t _elementSize )
    {
        m_name = _name;
        m_elementSize = _elementSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderVertexAttribute::finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MetalRenderVertexAttribute::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderVertexAttribute::getElementSize() const
    {
        return m_elementSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderVertexAttribute::addAttribute( const ConstString & _uniform, uint32_t _index, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset )
    {
        Attribute desc;
        desc.uniform = _uniform;
        desc.index = _index;
        desc.size = _size;
        desc.type = _type;
        desc.normalized = _normalized;
        desc.stride = _stride;
        desc.offset = _offset;

        m_attributes.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
}

