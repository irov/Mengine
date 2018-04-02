#include "DX9RenderVertexAttribute.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderVertexAttribute::DX9RenderVertexAttribute()
        : m_elementSize( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderVertexAttribute::~DX9RenderVertexAttribute()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexAttribute::initialize( const ConstString & _name, uint32_t _elementSize )
    {
        m_name = _name;
        m_elementSize = _elementSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DX9RenderVertexAttribute::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderVertexAttribute::getElementSize() const
    {
        return m_elementSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexAttribute::enable()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderVertexAttribute::disable()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderVertexAttribute::addAttribute( const ConstString & _uniform, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset )
    {
        Attribute attr;
        attr.uniform = _uniform;
        attr.size = _size;
        attr.type = _type;
        attr.normalized = _normalized;
        attr.stride = _stride;
        attr.offset = _offset;

        m_attributes.push_back( attr );
    }
}