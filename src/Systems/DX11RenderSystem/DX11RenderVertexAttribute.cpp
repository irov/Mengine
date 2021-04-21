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
    bool DX11RenderVertexAttribute::compile( ID3D11Device * _pD3DDevice, const void * _pShaderBytecodeWithInputSignature, uint32_t _bytecodeLength)
    {
        MENGINE_ASSERTION_FATAL( m_pD3DVertexDeclaration == nullptr );

        m_pD3DDevice = _pD3DDevice;

		D3D11_INPUT_ELEMENT_DESC declaration[64];

        DWORD declaration_iterator = 0;

        for( const AttributeDesc & desc : m_attributes )
        {
			D3D11_INPUT_ELEMENT_DESC& element = declaration[declaration_iterator++];

            if( desc.uniform == STRINGIZE_STRING_LOCAL( "inVert" ) )
            {
				element.InputSlot = 0;
				element.AlignedByteOffset = 0;
				element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				element.InstanceDataStepRate = 0;

				element.SemanticName = "Position"; // this name must be same in shader
				element.SemanticIndex = 0;
				element.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            }
            else if( desc.uniform == STRINGIZE_STRING_LOCAL( "inCol" ) )
            {
				element.InputSlot = 0;
				element.AlignedByteOffset = 12; // we could use D3D11_APPEND_ALIGNED_ELEMENT;
				element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				element.InstanceDataStepRate = 0;

				element.SemanticName = "Color"; // this name must be same in shader
				element.SemanticIndex = 0;
				element.Format = DXGI_FORMAT_R32_UINT;
            }
            else if( desc.uniform == STRINGIZE_STRING_LOCAL( "inUV0" ) )
            {
				element.InputSlot = 0;
				element.AlignedByteOffset = 16; // we could use D3D11_APPEND_ALIGNED_ELEMENT;
				element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				element.InstanceDataStepRate = 0;

				element.SemanticName = "TEXCOORD"; // this name must be same in shader
				element.SemanticIndex = 0;
				element.Format = DXGI_FORMAT_R32G32_FLOAT;
            }
            else if( desc.uniform == STRINGIZE_STRING_LOCAL( "inUV1" ) )
            {
				element.InputSlot = 0;
				element.AlignedByteOffset = 24; // we could use D3D11_APPEND_ALIGNED_ELEMENT;
				element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				element.InstanceDataStepRate = 0;

				element.SemanticName = "TEXCOORD"; // this name must be same in shader
				element.SemanticIndex = 1;
				element.Format = DXGI_FORMAT_R32G32_FLOAT;

            }
        }

        LOGGER_INFO( "render", "create vertex declaration '%s'"
            , m_name.c_str()
        );

        IF_DXCALL( m_pD3DDevice, CreateInputLayout, (declaration, declaration_iterator, _pShaderBytecodeWithInputSignature, _bytecodeLength, &m_pD3DVertexDeclaration) )
        {
            return false;
        }

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
    void DX11RenderVertexAttribute::enable(ID3D11DeviceContext * _pD3DDeviceContext)
    {
		_pD3DDeviceContext->IASetInputLayout(m_pD3DVertexDeclaration);
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexAttribute::disable(ID3D11DeviceContext * _pD3DDeviceContext)
    {
		_pD3DDeviceContext->IASetInputLayout(nullptr);
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