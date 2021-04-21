#include "MockupRenderVertexAttribute.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderVertexAttribute::MockupRenderVertexAttribute()
        : m_elementSize( 0 )        
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderVertexAttribute::~MockupRenderVertexAttribute()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderVertexAttribute::initialize( const ConstString & _name, uint32_t _elementSize )
    {
        m_name = _name;
        m_elementSize = _elementSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderVertexAttribute::finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MockupRenderVertexAttribute::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderVertexAttribute::getElementSize() const
    {
        return m_elementSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderVertexAttribute::addAttribute( const ConstString & _uniform, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset )
    {
        AttributeDesc desc;
        desc.uniform = _uniform;
        desc.size = _size;
        desc.type = _type;
        desc.stride = _stride;
        desc.offset = _offset;
        desc.normalized = _normalized;

        m_attributes.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
}