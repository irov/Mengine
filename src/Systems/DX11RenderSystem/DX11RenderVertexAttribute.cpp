#include "DX11RenderVertexAttribute.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"

#include "DX11RenderEnum.h"
#include "DX11ErrorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderVertexAttribute::DX11RenderVertexAttribute()
        : m_elementSize( 0 )
        , m_pD3DDevice( nullptr )
        , m_pD3DVertexDeclaration( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderVertexAttribute::~DX11RenderVertexAttribute()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DVertexDeclaration == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexAttribute::initialize( const ConstString & _name, uint32_t _elementSize )
    {
        m_name = _name;
        m_elementSize = _elementSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexAttribute::finalize()
    {
        DXRELEASE( m_pD3DVertexDeclaration );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexAttribute::compile( IDirect3DDevice9 * _pD3DDevice )
    {
        MENGINE_ASSERTION_FATAL( m_pD3DVertexDeclaration == nullptr );

        m_pD3DDevice = _pD3DDevice;

        D3DVERTEXELEMENT9 declaration[64];

        DWORD declaration_iterator = 0;

        for( const AttributeDesc & desc : m_attributes )
        {
            if( desc.uniform == STRINGIZE_STRING_LOCAL( "inVert" ) )
            {
                declaration[declaration_iterator++] = { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 };
            }
            else if( desc.uniform == STRINGIZE_STRING_LOCAL( "inCol" ) )
            {
                declaration[declaration_iterator++] = { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 };
            }
            else if( desc.uniform == STRINGIZE_STRING_LOCAL( "inUV0" ) )
            {
                declaration[declaration_iterator++] = { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 };
            }
            else if( desc.uniform == STRINGIZE_STRING_LOCAL( "inUV1" ) )
            {
                declaration[declaration_iterator++] = { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 };
            }
        }

        declaration[declaration_iterator] = D3DDECL_END();

        LOGGER_INFO( "render", "create vertex declaration '%s'"
            , m_name.c_str()
        );

        IDirect3DVertexDeclaration9 * pD3DVertexDeclaration;
        IF_DXCALL( m_pD3DDevice, CreateVertexDeclaration, (declaration, &pD3DVertexDeclaration) )
        {
            return false;
        }

        m_pD3DVertexDeclaration = pD3DVertexDeclaration;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DX11RenderVertexAttribute::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderVertexAttribute::getElementSize() const
    {
        return m_elementSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexAttribute::enable()
    {
        DXCALL( m_pD3DDevice, SetVertexDeclaration, (m_pD3DVertexDeclaration) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexAttribute::disable()
    {
        DXCALL( m_pD3DDevice, SetVertexDeclaration, (NULL) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexAttribute::addAttribute( const ConstString & _uniform, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset )
    {
        AttributeDesc desc;
        desc.uniform = _uniform;
        desc.size = _size;
        desc.type = _type;
        desc.normalized = _normalized;
        desc.stride = _stride;
        desc.offset = _offset;

        m_attributes.emplace_back( desc );

        MENGINE_ASSERTION_FATAL( m_attributes.size() < 64 );
    }
    //////////////////////////////////////////////////////////////////////////
}