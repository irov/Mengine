#include "MCPProtocol.h"

#include "Kernel/ReadHelper.h"
#include "Kernel/WriteHelper.h"

#include "Config/StdString.h"
#include "Config/StdUtility.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPFrameDecoder::MCPFrameDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPFrameDecoder::~MCPFrameDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFrameDecoder::append( const void * _buffer, size_t _size, Vector<MCPFrame> * const _frames, const Char ** const _error )
    {
        const uint8_t * bytes = static_cast<const uint8_t *>(_buffer);
        m_buffer.insert( m_buffer.end(), bytes, bytes + _size );

        size_t offset = 0;

        while( m_buffer.size() - offset >= MCP_PROTOCOL_HEADER_SIZE )
        {
            const uint8_t * header = m_buffer.data() + offset;

            if( StdString::memcmp( header, "MNCP", 4 ) != 0 )
            {
                *_error = "invalid MNCP magic";
                return false;
            }

            uint16_t version;
            Helper::readUint16( header + 4, &version );

            if( version != MCP_PROTOCOL_VERSION )
            {
                *_error = "unsupported MNCP version";
                return false;
            }

            uint8_t rawType = header[6];
            if( rawType < (uint8_t)EMCPFrameType::Request || rawType > (uint8_t)EMCPFrameType::Cancel )
            {
                *_error = "invalid MNCP frame type";
                return false;
            }

            uint32_t payloadSize;
            Helper::readUint32( header + 12, &payloadSize );

            if( payloadSize > MCP_PROTOCOL_MAX_PAYLOAD )
            {
                *_error = "MNCP payload exceeds limit";
                return false;
            }

            uint32_t chunkIndex;
            Helper::readUint32( header + 16, &chunkIndex );

            uint32_t chunkCount;
            Helper::readUint32( header + 20, &chunkCount );

            if( chunkCount == 0 || chunkIndex >= chunkCount )
            {
                *_error = "invalid MNCP chunk coordinates";
                return false;
            }

            size_t frameSize = MCP_PROTOCOL_HEADER_SIZE + payloadSize;
            if( m_buffer.size() - offset < frameSize )
            {
                break;
            }

            MCPFrame frame;
            frame.type = (EMCPFrameType)rawType;
            frame.flags = header[7];
            Helper::readUint32( header + 8, &frame.requestId );
            frame.chunkIndex = chunkIndex;
            frame.chunkCount = chunkCount;
            frame.payload.assign( header + MCP_PROTOCOL_HEADER_SIZE, header + frameSize );

            _frames->emplace_back( StdUtility::move( frame ) );
            offset += frameSize;
        }

        if( offset != 0 )
        {
            m_buffer.erase( m_buffer.begin(), m_buffer.begin() + offset );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFrameDecoder::reset()
    {
        m_buffer.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        Data makeMCPFrame( EMCPFrameType _type, uint8_t _flags, uint32_t _requestId, uint32_t _chunkIndex, uint32_t _chunkCount, const void * _payload, size_t _payloadSize )
        {
            Data frame;
            frame.resize( MCP_PROTOCOL_HEADER_SIZE + _payloadSize );

            uint8_t * header = frame.data();
            StdString::memcpy( header, "MNCP", 4 );
            Helper::writeUint16( header + 4, MCP_PROTOCOL_VERSION );
            header[6] = (uint8_t)_type;
            header[7] = _flags;
            Helper::writeUint32( header + 8, _requestId );
            Helper::writeUint32( header + 12, (uint32_t)_payloadSize );
            Helper::writeUint32( header + 16, _chunkIndex );
            Helper::writeUint32( header + 20, _chunkCount );

            if( _payloadSize != 0 )
            {
                StdString::memcpy( header + MCP_PROTOCOL_HEADER_SIZE, _payload, _payloadSize );
            }

            return frame;
        }
        //////////////////////////////////////////////////////////////////////////
        Data makeMCPJSONFrame( EMCPFrameType _type, uint32_t _requestId, const void * _payload, size_t _payloadSize )
        {
            Data frame = Helper::makeMCPFrame( _type, MCP_FRAME_FLAG_JSON | MCP_FRAME_FLAG_FINAL, _requestId, 0, 1, _payload, _payloadSize );

            return frame;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
