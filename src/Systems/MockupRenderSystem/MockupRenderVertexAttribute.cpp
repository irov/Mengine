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
    void MockupRenderVertexAttribute::addAttribute( const ConstString & _uniform, uint32_t _index, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset )
    {
        MENGINE_UNUSED( _uniform );
        MENGINE_UNUSED( _index );
        MENGINE_UNUSED( _size );
        MENGINE_UNUSED( _type );
        MENGINE_UNUSED( _normalized );
        MENGINE_UNUSED( _stride );
        MENGINE_UNUSED( _offset );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}