#include "DX11RenderProgramVariable.h"

#include "DX11ErrorHelper.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderProgramVariable::DX11RenderProgramVariable() 
		: m_vertexBufferUpdate(false)
		, m_pixelBufferUpdate(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderProgramVariable::~DX11RenderProgramVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderProgramVariable::initialize( uint32_t _vertexCount, uint32_t _pixelCount )
    {
        m_vertexVariables.resize( _vertexCount );
        m_pixelVariables.resize( _pixelCount );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::finalize()
    {
        m_vertexVariables.clear();
        m_pixelVariables.clear();

		for (size_t i = 0; i < m_vertexBuffers.size(); ++i)
			m_vertexBuffers[i]->Release();

		for (size_t i = 0; i < m_pixelBuffers.size(); ++i)
			m_pixelBuffers[i]->Release();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setVertexVariableFloats( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

		// let's check first index - must be float4x4
		if (_index == 0 && _size * _count != 16)
		{
			// TODO: warning to log
		}

        ProgramVariableDesc variable;

		variable.offset = (uint32_t)m_vertexFloats.size();
		variable.count = _size * _count;

		m_vertexFloats.insert(m_vertexFloats.end(), _values, _values + _size * _count);

        m_vertexVariables[_index] = variable;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setVertexVariableIntegers( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
		MENGINE_UNUSED(_uniform);
		MENGINE_UNUSED(_index);
		MENGINE_UNUSED(_values);
		MENGINE_UNUSED(_size);
		MENGINE_UNUSED(_count);
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setVertexVariableBooleans( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
		MENGINE_UNUSED(_uniform);
		MENGINE_UNUSED(_index);
		MENGINE_UNUSED(_values);
		MENGINE_UNUSED(_size);
		MENGINE_UNUSED(_count);
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setPixelVariableFloats( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
		MENGINE_UNUSED(_uniform);

		ProgramVariableDesc variable;

		variable.offset = (uint32_t)m_pixelFloats.size();
		variable.count = _count;
		variable.size = _size;

		m_pixelFloats.insert(m_pixelFloats.end(), _values, _values + _size * _count);

		m_pixelVariables[_index] = variable;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setPixelVariableIntegers( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
		MENGINE_UNUSED(_uniform);
		MENGINE_UNUSED(_index);
		MENGINE_UNUSED(_values);
		MENGINE_UNUSED(_size);
		MENGINE_UNUSED(_count);
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setPixelVariableBooleans( const Char * _uniform, uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
		MENGINE_UNUSED(_uniform);
		MENGINE_UNUSED(_index);
		MENGINE_UNUSED(_values);
		MENGINE_UNUSED(_size);
		MENGINE_UNUSED(_count);
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::updatePixelVariableFloats( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        const ProgramVariableDesc & v = m_pixelVariables[_index];

        float * values = &m_pixelFloats[v.offset];
        std::copy( _values, _values + _size * _count, values );

		m_pixelBufferUpdate = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::updatePixelVariableIntegers( uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
		MENGINE_UNUSED(_index);
		MENGINE_UNUSED(_values);
		MENGINE_UNUSED(_size);
		MENGINE_UNUSED(_count);
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::updatePixelVariableBooleans( uint32_t _index, const int32_t * _values, uint32_t _size, uint32_t _count )
    {
		MENGINE_UNUSED(_index);
		MENGINE_UNUSED(_values);
		MENGINE_UNUSED(_size);
		MENGINE_UNUSED(_count);
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderProgramVariable::apply( ID3D11Device * _pD3DDevice, const RenderProgramInterfacePtr & _program )
    {
        MENGINE_UNUSED( _program );

		ID3D11DeviceContext *pImmediateContext = nullptr;
		_pD3DDevice->GetImmediateContext(&pImmediateContext);

		// create vertex shader buffers
		if (m_vertexVariables.size() != m_vertexBuffers.size())
		{
			m_vertexBufferUpdate = false;

			D3D11_BUFFER_DESC descConstBuffer;

			descConstBuffer.Usage = D3D11_USAGE_DYNAMIC;
			descConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			descConstBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			descConstBuffer.MiscFlags = 0;
			descConstBuffer.StructureByteStride = 0;

			m_vertexBuffers.resize(m_vertexVariables.size());

			uint32_t vertexEnumerator = 0;
			for (const ProgramVariableDesc & v : m_vertexVariables)
			{
				auto* Buffer11 = &m_vertexBuffers[vertexEnumerator];
				descConstBuffer.ByteWidth = v.count * v.size;

				float * values = &m_vertexFloats[v.offset];

				D3D11_SUBRESOURCE_DATA InitData;
				InitData.pSysMem = values;
				InitData.SysMemPitch = 0;
				InitData.SysMemSlicePitch = 0;

				IF_DXCALL(_pD3DDevice, CreateBuffer, (&descConstBuffer, &InitData, Buffer11))
				{
					return false;
				}

				++vertexEnumerator;
			}
		}

		if (m_vertexBufferUpdate)
		{

		}

		// set all vertex buffers
		pImmediateContext->VSSetConstantBuffers(0, m_vertexBuffers.size(), &m_vertexBuffers[0]);

		// create pixel shader buffers
		if (m_pixelVariables.size() != m_pixelBuffers.size())
		{
			m_pixelBufferUpdate = false;

			D3D11_BUFFER_DESC descConstBuffer;

			descConstBuffer.Usage = D3D11_USAGE_DYNAMIC;
			descConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			descConstBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			descConstBuffer.MiscFlags = 0;
			descConstBuffer.StructureByteStride = 0;

			m_pixelBuffers.resize(m_pixelVariables.size());

			uint32_t vertexEnumerator = 0;
			for (const ProgramVariableDesc & v : m_pixelVariables)
			{
				auto* Buffer11 = &m_pixelBuffers[vertexEnumerator];
				descConstBuffer.ByteWidth = v.count * v.size;

				float * values = &m_pixelFloats[v.offset];

				D3D11_SUBRESOURCE_DATA InitData;
				InitData.pSysMem = values;
				InitData.SysMemPitch = 0;
				InitData.SysMemSlicePitch = 0;

				IF_DXCALL(_pD3DDevice, CreateBuffer, (&descConstBuffer, &InitData, Buffer11))
				{
					return false;
				}

				++vertexEnumerator;
			}
		}

		if (m_pixelBufferUpdate)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;

			uint32_t vertexEnumerator = 0;
			for (const ProgramVariableDesc & v : m_pixelVariables)
			{
				auto Buffer11 = m_pixelBuffers[vertexEnumerator];
				float * values = &m_pixelFloats[v.offset];

				auto hResult = pImmediateContext->Map(Buffer11, 0, D3D11_MAP_WRITE, 0, &mappedResource);
				if (FAILED(hResult))
				{
					// TODO: add error log
					return nullptr;
				}

				stdex::memorycopy(mappedResource.pData, 0, values, v.count * v.size);

				pImmediateContext->Unmap(Buffer11, 0);
				++vertexEnumerator;
			}
		}

		// set all pixel buffers
		pImmediateContext->PSSetConstantBuffers(0, m_pixelBuffers.size(), &m_pixelBuffers[0]);

		pImmediateContext->Release();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}