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
        , m_compileReferenceCount( 0 )
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
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexAttribute::compile( const ID3D11DevicePtr & _pD3DDevice, const MemoryInterfacePtr & _shaderCompileMemory )
    {
        if( m_compileReferenceCount == 0 )
        {
            MENGINE_ASSERTION_FATAL( m_pD3DVertexDeclaration == nullptr );

            D3D11_INPUT_ELEMENT_DESC declaration[64];

            DWORD declaration_iterator = 0;

            for( const AttributeDesc & desc : m_attributes )
            {
                D3D11_INPUT_ELEMENT_DESC & element = declaration[declaration_iterator++];

                element.SemanticName = desc.uniform.c_str();
                element.SemanticIndex = desc.index;

                switch( desc.type )
                {
                case VAT_FLOAT:
                    {
                        if( desc.size == 2)
                        {
                            element.Format = DXGI_FORMAT_R32G32_FLOAT;
                        }
                        else if( desc.size == 3 )
                        {
                            element.Format = DXGI_FORMAT_R32G32B32_FLOAT;
                        }
                    }break;
                case VAT_UNSIGNED_BYTE:
                    {
                        if( desc.size == 4 && desc.normalized == true )
                        {
                            element.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                        }
                    }break;
                }
                
                element.InputSlot = 0;
                element.AlignedByteOffset = desc.offset;
                element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                element.InstanceDataStepRate = 0;
            }

            LOGGER_INFO( "render", "create vertex declaration '%s'"
                , m_name.c_str()
            );

            const void * shaderCompileMemoryBuffer = _shaderCompileMemory->getBuffer();
            size_t shaderCompileMemorySize = _shaderCompileMemory->getSize();

            ID3D11InputLayout * pD3DVertexDeclaration;
            IF_DXCALL( _pD3DDevice, CreateInputLayout, (declaration, declaration_iterator, shaderCompileMemoryBuffer, shaderCompileMemorySize, &pD3DVertexDeclaration) )
            {
                return false;
            }

            m_pD3DVertexDeclaration = pD3DVertexDeclaration;
        }

        ++m_compileReferenceCount;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexAttribute::release()
    {
        --m_compileReferenceCount;

        if( m_compileReferenceCount == 0 )
        {
            m_pD3DVertexDeclaration = nullptr;
        }
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
    void DX11RenderVertexAttribute::enable( const ID3D11DeviceContextPtr & _pD3DDeviceContext )
    {
        _pD3DDeviceContext->IASetInputLayout( m_pD3DVertexDeclaration.Get() );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexAttribute::disable( const ID3D11DeviceContextPtr & _pD3DDeviceContext )
    {
        _pD3DDeviceContext->IASetInputLayout( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexAttribute::addAttribute( const ConstString & _uniform, uint32_t _index, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset )
    {
        AttributeDesc desc;
        desc.uniform = _uniform;
        desc.index = _index;
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