#include "DX9RenderVertexAttribute.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"

#include "DX9RenderEnum.h"
#include "DX9RenderErrorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderVertexAttribute::DX9RenderVertexAttribute()
        : m_elementSize( 0 )
        , m_pD3DVertexDeclaration( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderVertexAttribute::~DX9RenderVertexAttribute()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DVertexDeclaration == nullptr, "vertex declaration not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexAttribute::initialize( const ConstString & _name, uint32_t _elementSize )
    {
        m_name = _name;
        m_elementSize = _elementSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderVertexAttribute::finalize()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DVertexDeclaration == nullptr, "vertex declaration not released" );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexAttribute::compile( IDirect3DDevice9 * _pD3DDevice )
    {
        uint32_t referenceCount = m_compileReferenceCount.increfReferenceCount();

        if( referenceCount == 0 )
        {
            MENGINE_ASSERTION_FATAL( m_pD3DVertexDeclaration == nullptr, "vertex declaration is already compile" );

            D3DVERTEXELEMENT9 declaration[64];

            DWORD declaration_iterator = 0;

            for( const AttributeDesc & desc : m_attributes )
            {
                if( desc.uniform == STRINGIZE_STRING_LOCAL( "inVert" ) )
                {
                    declaration[declaration_iterator++] = {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0};
                }
                else if( desc.uniform == STRINGIZE_STRING_LOCAL( "inCol" ) )
                {
                    declaration[declaration_iterator++] = {0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0};
                }
                else if( desc.uniform == STRINGIZE_STRING_LOCAL( "inUV0" ) )
                {
                    declaration[declaration_iterator++] = {0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0};
                }
                else if( desc.uniform == STRINGIZE_STRING_LOCAL( "inUV1" ) )
                {
                    declaration[declaration_iterator++] = {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1};
                }
            }

            declaration[declaration_iterator] = D3DDECL_END();

            LOGGER_INFO( "render", "create vertex declaration '%s'"
                , m_name.c_str()
            );

            IDirect3DVertexDeclaration9 * pD3DVertexDeclaration;
            MENGINE_IF_DX9_CALL( _pD3DDevice, CreateVertexDeclaration, (declaration, &pD3DVertexDeclaration) )
            {
                return false;
            }

            m_pD3DVertexDeclaration = pD3DVertexDeclaration;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderVertexAttribute::release()
    {
        uint32_t referenceCount = m_compileReferenceCount.decrefReferenceCount();

        if( referenceCount == 0 )
        {
            MENGINE_DX9_RELEASE( m_pD3DVertexDeclaration );
        }
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
    void DX9RenderVertexAttribute::enable( IDirect3DDevice9 * _pD3DDevice )
    {
        MENGINE_DX9_CALL( _pD3DDevice, SetVertexDeclaration, (m_pD3DVertexDeclaration) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderVertexAttribute::disable( IDirect3DDevice9 * _pD3DDevice )
    {
        MENGINE_DX9_CALL( _pD3DDevice, SetVertexDeclaration, (NULL) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderVertexAttribute::addAttribute( const ConstString & _uniform, uint32_t _index, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset )
    {
        MENGINE_UNUSED( _index );

        AttributeDesc desc;
        desc.uniform = _uniform;
        desc.size = _size;
        desc.type = _type;
        desc.normalized = _normalized;
        desc.stride = _stride;
        desc.offset = _offset;

        m_attributes.emplace_back( desc );

        MENGINE_ASSERTION_FATAL( m_attributes.size() < 64, "too many attributes" );
    }
    //////////////////////////////////////////////////////////////////////////
}