#include "DX9RenderVertexAttribute.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"

#include "DX9RenderEnum.h"
#include "DX9RenderErrorHelper.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct UniformSemantic
        {
            D3DDECLUSAGE usage;
            BYTE usageIndex;
        };
        //////////////////////////////////////////////////////////////////////////
        static bool resolveUniformSemantic( const ConstString & _uniform, UniformSemantic * const _out )
        {
            if( _uniform == STRINGIZE_STRING_LOCAL( "inVert" ) )
            {
                *_out = {D3DDECLUSAGE_POSITION, 0};
                return true;
            }
            if( _uniform == STRINGIZE_STRING_LOCAL( "inNormal" ) )
            {
                *_out = {D3DDECLUSAGE_NORMAL, 0};
                return true;
            }
            if( _uniform == STRINGIZE_STRING_LOCAL( "inTangent" ) )
            {
                *_out = {D3DDECLUSAGE_TANGENT, 0};
                return true;
            }
            if( _uniform == STRINGIZE_STRING_LOCAL( "inBitangent" ) )
            {
                *_out = {D3DDECLUSAGE_BINORMAL, 0};
                return true;
            }
            if( _uniform == STRINGIZE_STRING_LOCAL( "inCol" ) )
            {
                *_out = {D3DDECLUSAGE_COLOR, 0};
                return true;
            }
            if( _uniform == STRINGIZE_STRING_LOCAL( "inUV0" ) )
            {
                *_out = {D3DDECLUSAGE_TEXCOORD, 0};
                return true;
            }
            if( _uniform == STRINGIZE_STRING_LOCAL( "inUV1" ) )
            {
                *_out = {D3DDECLUSAGE_TEXCOORD, 1};
                return true;
            }
            if( _uniform == STRINGIZE_STRING_LOCAL( "inUV2" ) )
            {
                *_out = {D3DDECLUSAGE_TEXCOORD, 2};
                return true;
            }
            if( _uniform == STRINGIZE_STRING_LOCAL( "inUV3" ) )
            {
                *_out = {D3DDECLUSAGE_TEXCOORD, 3};
                return true;
            }
            if( _uniform == STRINGIZE_STRING_LOCAL( "inBoneIndices" ) )
            {
                *_out = {D3DDECLUSAGE_BLENDINDICES, 0};
                return true;
            }
            if( _uniform == STRINGIZE_STRING_LOCAL( "inBoneWeights" ) )
            {
                *_out = {D3DDECLUSAGE_BLENDWEIGHT, 0};
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool resolveDeclType( EVertexAttributeType _type, uint32_t _size, bool _normalized, D3DDECLTYPE * const _out )
        {
            switch( _type )
            {
            case VAT_FLOAT:
                {
                    switch( _size )
                    {
                    case 1: *_out = D3DDECLTYPE_FLOAT1; return true;
                    case 2: *_out = D3DDECLTYPE_FLOAT2; return true;
                    case 3: *_out = D3DDECLTYPE_FLOAT3; return true;
                    case 4: *_out = D3DDECLTYPE_FLOAT4; return true;
                    default: return false;
                    }
                }break;
            case VAT_UNSIGNED_BYTE:
                {
                    if( _size != 4 )
                    {
                        return false;
                    }

                    *_out = _normalized == true ? D3DDECLTYPE_D3DCOLOR : D3DDECLTYPE_UBYTE4;

                    return true;
                }break;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
    }
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
                Detail::UniformSemantic semantic;
                if( Detail::resolveUniformSemantic( desc.uniform, &semantic ) == false )
                {
                    LOGGER_ASSERTION( "vertex attribute '%s': unknown uniform '%s'"
                        , m_name.c_str()
                        , desc.uniform.c_str()
                    );

                    return false;
                }

                D3DDECLTYPE declType;
                if( Detail::resolveDeclType( desc.type, desc.size, desc.normalized, &declType ) == false )
                {
                    LOGGER_ASSERTION( "vertex attribute '%s' uniform '%s': unsupported (type %u size %u normalized %u)"
                        , m_name.c_str()
                        , desc.uniform.c_str()
                        , (uint32_t)desc.type
                        , desc.size
                        , (uint32_t)(desc.normalized ? 1 : 0)
                    );

                    return false;
                }

                declaration[declaration_iterator++] = {0, (WORD)desc.offset, (BYTE)declType, D3DDECLMETHOD_DEFAULT, (BYTE)semantic.usage, semantic.usageIndex};
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