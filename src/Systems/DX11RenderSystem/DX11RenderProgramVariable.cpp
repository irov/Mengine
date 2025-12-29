#include "DX11RenderProgramVariable.h"

#include "DX11RenderErrorHelper.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderProgramVariable::DX11RenderProgramVariable()
        : m_vertexBufferUpdate( false )
        , m_pixelBufferUpdate( false )
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
        m_vertexFloats.clear();
        m_pixelFloats.clear();

        m_vertexVariables.clear();
        m_pixelVariables.clear();

        for( ID3D11Buffer * buffer : m_vertexBuffers )
        {
            buffer->Release();
        }

        m_vertexBuffers.clear();

        for( ID3D11Buffer * buffer : m_pixelBuffers )
        {
            buffer->Release();
        }

        m_pixelBuffers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        // let's check first index - must be float4x4
        if( _index == 0 && _size * _count != 16 )
        {
            // TODO: warning to log
        }

        m_vertexFloats.insert( m_vertexFloats.end(), _values, _values + _size * _count );

        ProgramVariableDesc variable;

        variable.offset = (uint32_t)m_vertexFloats.size();
        variable.size = _size;
        variable.count = _count;

        m_vertexVariables[_index] = variable;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        m_pixelFloats.insert( m_pixelFloats.end(), _values, _values + _size * _count );

        ProgramVariableDesc variable;

        variable.offset = (uint32_t)m_pixelFloats.size();
        variable.size = _size;
        variable.count = _count;

        m_pixelVariables[_index] = variable;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::updatePixelVariables( uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        const ProgramVariableDesc & v = m_pixelVariables[_index];

        float * values = m_pixelFloats.data() + v.offset;
        std::copy( _values, _values + _size * _count, values );

        m_pixelBufferUpdate = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderProgramVariable::apply( const ID3D11DevicePtr & _pD3DDevice, const ID3D11DeviceContextPtr & _pImmediateContext )
    {
        // create vertex shader buffers
        if( m_vertexVariables.size() != m_vertexBuffers.size() )
        {
            m_vertexBufferUpdate = false;

            D3D11_BUFFER_DESC descConstBuffer;

            descConstBuffer.Usage = D3D11_USAGE_DYNAMIC;
            descConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            descConstBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            descConstBuffer.MiscFlags = 0;
            descConstBuffer.StructureByteStride = 0;

            m_vertexBuffers.resize( m_vertexVariables.size() );

            uint32_t vertexEnumerator = 0;
            for( const ProgramVariableDesc & v : m_vertexVariables )
            {
                descConstBuffer.ByteWidth = v.count * v.size;

                const float * values = m_vertexFloats.data() + v.offset;

                D3D11_SUBRESOURCE_DATA InitData;
                InitData.pSysMem = values;
                InitData.SysMemPitch = 0;
                InitData.SysMemSlicePitch = 0;

                ID3D11Buffer * d3dBuffer;
                MENGINE_IF_DX11_CALL( _pD3DDevice, CreateBuffer, (&descConstBuffer, &InitData, &d3dBuffer) )
                {
                    return false;
                }

                m_vertexBuffers[vertexEnumerator] = d3dBuffer;

                ++vertexEnumerator;
            }
        }

        if( m_vertexBufferUpdate == true )
        {

        }

        // set all vertex buffers
        if( m_vertexBuffers.empty() == false )
        {
            ID3D11Buffer ** buffers = m_vertexBuffers.data();
            UINT count = (UINT)m_vertexBuffers.size();

            _pImmediateContext->VSSetConstantBuffers( 0, count, buffers );
        }

        // create pixel shader buffers
        if( m_pixelVariables.size() != m_pixelBuffers.size() )
        {
            m_pixelBufferUpdate = false;

            D3D11_BUFFER_DESC descConstBuffer;

            descConstBuffer.Usage = D3D11_USAGE_DYNAMIC;
            descConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            descConstBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            descConstBuffer.MiscFlags = 0;
            descConstBuffer.StructureByteStride = 0;

            m_pixelBuffers.resize( m_pixelVariables.size() );

            uint32_t vertexEnumerator = 0;
            for( const ProgramVariableDesc & v : m_pixelVariables )
            {
                descConstBuffer.ByteWidth = v.count * v.size;

                const float * values = m_pixelFloats.data() + v.offset;

                D3D11_SUBRESOURCE_DATA InitData;
                InitData.pSysMem = values;
                InitData.SysMemPitch = 0;
                InitData.SysMemSlicePitch = 0;

                ID3D11Buffer * d3dBuffer;
                MENGINE_IF_DX11_CALL( _pD3DDevice, CreateBuffer, (&descConstBuffer, &InitData, &d3dBuffer) )
                {
                    return false;
                }

                m_pixelBuffers[vertexEnumerator] = d3dBuffer;

                ++vertexEnumerator;
            }
        }

        if( m_pixelBufferUpdate == true )
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource;

            uint32_t vertexEnumerator = 0;
            for( const ProgramVariableDesc & v : m_pixelVariables )
            {
                ID3D11Buffer * d3dBuffer = m_pixelBuffers[vertexEnumerator];

                MENGINE_IF_DX11_CALL( _pImmediateContext, Map, (d3dBuffer, 0, D3D11_MAP_WRITE, 0, &mappedResource) )
                {
                    return nullptr;
                }

                const float * values = m_pixelFloats.data() + v.offset;

                stdex::memorycopy( mappedResource.pData, 0, values, v.count * v.size );

                _pImmediateContext->Unmap( d3dBuffer, 0 );

                ++vertexEnumerator;
            }
        }

        // set all pixel buffers
        if( m_pixelBuffers.empty() == false )
        {
            ID3D11Buffer ** buffers = m_pixelBuffers.data();
            UINT count = (UINT)m_pixelBuffers.size();

            _pImmediateContext->PSSetConstantBuffers( 0, count, buffers );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}