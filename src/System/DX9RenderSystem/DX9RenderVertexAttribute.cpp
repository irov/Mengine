#include "DX9RenderVertexAttribute.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderVertexAttribute::DX9RenderVertexAttribute()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderVertexAttribute::~DX9RenderVertexAttribute()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexAttribute::initialize( const ConstString & _name )
    {
        m_name = _name;

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
        uint32_t vertexSize = 0;

        for( TVectorAttribute::const_iterator
            it = m_attributes.begin(),
            it_end = m_attributes.end();
            it != it_end;
            ++it )
        {
            const Attribute & attribute = *it;

            switch( attribute.type )
            {
            case VAT_FLOAT:
                {
                    vertexSize += attribute.size * sizeof( float );
                }break;
            case VAT_UNSIGNED_BYTE:
                {
                    vertexSize += attribute.size * sizeof( unsigned char );
                }break;
            }
        }

        return vertexSize;
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