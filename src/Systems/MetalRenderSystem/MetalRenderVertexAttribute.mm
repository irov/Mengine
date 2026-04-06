#include "MetalRenderVertexAttribute.h"

#include "Kernel/Logger.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderVertexAttribute::MetalRenderVertexAttribute()
        : m_elementSize( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderVertexAttribute::~MetalRenderVertexAttribute()
    {
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
        AttributeDesc attr;
        attr.uniform = _uniform;
        attr.index = _index;
        attr.size = _size;
        attr.type = _type;
        attr.stride = _stride;
        attr.offset = _offset;
        attr.normalized = _normalized;

        m_attributes.emplace_back( attr );
    }
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
        m_attributes.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    const MetalRenderVertexAttribute::VectorAttribute & MetalRenderVertexAttribute::getAttributes() const
    {
        return m_attributes;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderVertexAttribute::onRenderReset()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderVertexAttribute::onRenderRestore()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
