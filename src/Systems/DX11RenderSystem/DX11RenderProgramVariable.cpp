#include "DX11RenderProgramVariable.h"

#include <algorithm>

#include "DX11RenderErrorHelper.h"

#include "Kernel/MemoryCopy.h"

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
            if( buffer != nullptr )
            {
                buffer->Release();
            }
        }

        m_vertexBuffers.clear();

        for( ID3D11Buffer * buffer : m_pixelBuffers )
        {
            if( buffer != nullptr )
            {
                buffer->Release();
            }
        }

        m_pixelBuffers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setVertexVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        if( _index >= m_vertexVariables.size() )
        {
            m_vertexVariables.resize( _index + 1 );
        }

        ProgramVariableDesc & current = m_vertexVariables[_index];
        if( current.count != 0 && current.size == _size && current.count == _count )
        {
            std::copy( _values, _values + _size * _count, m_vertexFloats.data() + current.offset );
            m_vertexBufferUpdate = true;
            return;
        }

        if( current.count != 0 )
        {
            for( ID3D11Buffer * buffer : m_vertexBuffers )
            {
                if( buffer != nullptr ) buffer->Release();
            }
            m_vertexBuffers.clear();
        }

        ProgramVariableDesc variable;

        variable.offset = (uint32_t)m_vertexFloats.size();
        variable.size = _size;
        variable.count = _count;

        m_vertexFloats.insert( m_vertexFloats.end(), _values, _values + _size * _count );
        m_vertexVariables[_index] = variable;
        m_vertexBufferUpdate = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderProgramVariable::setPixelVariables( const Char * _uniform, uint32_t _index, const float * _values, uint32_t _size, uint32_t _count )
    {
        MENGINE_UNUSED( _uniform );

        if( _index >= m_pixelVariables.size() )
        {
            m_pixelVariables.resize( _index + 1 );
        }

        ProgramVariableDesc & current = m_pixelVariables[_index];
        if( current.count != 0 && current.size == _size && current.count == _count )
        {
            std::copy( _values, _values + _size * _count, m_pixelFloats.data() + current.offset );
            m_pixelBufferUpdate = true;
            return;
        }

        if( current.count != 0 )
        {
            for( ID3D11Buffer * buffer : m_pixelBuffers )
            {
                if( buffer != nullptr ) buffer->Release();
            }
            m_pixelBuffers.clear();
        }

        ProgramVariableDesc variable;

        variable.offset = (uint32_t)m_pixelFloats.size();
        variable.size = _size;
        variable.count = _count;

        m_pixelFloats.insert( m_pixelFloats.end(), _values, _values + _size * _count );
        m_pixelVariables[_index] = variable;
        m_pixelBufferUpdate = true;
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
        if( m_vertexVariables.size() != m_vertexBuffers.size() )
        {
            for( ID3D11Buffer * buffer : m_vertexBuffers )
            {
                if( buffer != nullptr )
                {
                    buffer->Release();
                }
            }

            D3D11_BUFFER_DESC descConstBuffer;

            descConstBuffer.Usage = D3D11_USAGE_DYNAMIC;
            descConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            descConstBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            descConstBuffer.MiscFlags = 0;
            descConstBuffer.StructureByteStride = 0;

            m_vertexBuffers.assign( m_vertexVariables.size(), nullptr );

            uint32_t vertexEnumerator = 0;
            for( const ProgramVariableDesc & v : m_vertexVariables )
            {
                if( v.count == 0 )
                {
                    ++vertexEnumerator;
                    continue;
                }

                const uint32_t dataBytes = v.count * v.size * (uint32_t)sizeof( float );
                descConstBuffer.ByteWidth = (dataBytes + 15u) & ~15u;

                ID3D11Buffer * d3dBuffer;
                MENGINE_IF_DX11_CALL( _pD3DDevice, CreateBuffer, (&descConstBuffer, nullptr, &d3dBuffer) )
                {
                    return false;
                }

                m_vertexBuffers[vertexEnumerator] = d3dBuffer;

                ++vertexEnumerator;
            }

            m_vertexBufferUpdate = true;
        }

        if( m_vertexBufferUpdate == true )
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource;

            for( uint32_t index = 0; index != m_vertexVariables.size(); ++index )
            {
                ID3D11Buffer * buffer = m_vertexBuffers[index];
                if( buffer == nullptr )
                {
                    continue;
                }

                const ProgramVariableDesc & v = m_vertexVariables[index];
                MENGINE_IF_DX11_CALL( _pImmediateContext, Map, (buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) )
                {
                    return false;
                }

                const float * values = m_vertexFloats.data() + v.offset;
                Helper::memoryCopy( mappedResource.pData, 0, values, 0, v.count * v.size * sizeof( float ) );
                _pImmediateContext->Unmap( buffer, 0 );
            }

            m_vertexBufferUpdate = false;
        }

        for( uint32_t index = 0; index != m_vertexBuffers.size(); ++index )
        {
            ID3D11Buffer * buffer = m_vertexBuffers[index];
            if( buffer != nullptr )
            {
                _pImmediateContext->VSSetConstantBuffers( index, 1, &buffer );
            }
        }

        if( m_pixelVariables.size() != m_pixelBuffers.size() )
        {
            for( ID3D11Buffer * buffer : m_pixelBuffers )
            {
                if( buffer != nullptr )
                {
                    buffer->Release();
                }
            }

            D3D11_BUFFER_DESC descConstBuffer;

            descConstBuffer.Usage = D3D11_USAGE_DYNAMIC;
            descConstBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            descConstBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            descConstBuffer.MiscFlags = 0;
            descConstBuffer.StructureByteStride = 0;

            m_pixelBuffers.assign( m_pixelVariables.size(), nullptr );

            uint32_t vertexEnumerator = 0;
            for( const ProgramVariableDesc & v : m_pixelVariables )
            {
                if( v.count == 0 )
                {
                    ++vertexEnumerator;
                    continue;
                }

                const uint32_t dataBytes = v.count * v.size * (uint32_t)sizeof( float );
                descConstBuffer.ByteWidth = (dataBytes + 15u) & ~15u;

                ID3D11Buffer * d3dBuffer;
                MENGINE_IF_DX11_CALL( _pD3DDevice, CreateBuffer, (&descConstBuffer, nullptr, &d3dBuffer) )
                {
                    return false;
                }

                m_pixelBuffers[vertexEnumerator] = d3dBuffer;

                ++vertexEnumerator;
            }

            m_pixelBufferUpdate = true;
        }

        if( m_pixelBufferUpdate == true )
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource;

            uint32_t vertexEnumerator = 0;
            for( const ProgramVariableDesc & v : m_pixelVariables )
            {
                ID3D11Buffer * d3dBuffer = m_pixelBuffers[vertexEnumerator];
                if( d3dBuffer == nullptr )
                {
                    ++vertexEnumerator;
                    continue;
                }

                MENGINE_IF_DX11_CALL( _pImmediateContext, Map, (d3dBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) )
                {
                    return false;
                }

                const float * values = m_pixelFloats.data() + v.offset;

                Helper::memoryCopy( mappedResource.pData, 0, values, 0, v.count * v.size * sizeof( float ) );

                _pImmediateContext->Unmap( d3dBuffer, 0 );

                ++vertexEnumerator;
            }

            m_pixelBufferUpdate = false;
        }

        for( uint32_t index = 0; index != m_pixelBuffers.size(); ++index )
        {
            ID3D11Buffer * buffer = m_pixelBuffers[index];
            if( buffer != nullptr )
            {
                _pImmediateContext->PSSetConstantBuffers( index, 1, &buffer );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
