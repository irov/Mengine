#pragma once

#include "Kernel/Data.h"
#include "Kernel/Vector.h"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint16_t MCP_PROTOCOL_VERSION = 1;
    static constexpr size_t MCP_PROTOCOL_HEADER_SIZE = 24;
    static constexpr uint32_t MCP_PROTOCOL_MAX_PAYLOAD = 16 * 1024 * 1024;
    static constexpr size_t MCP_PROTOCOL_MAX_ATTACHMENT = 256 * 1024 * 1024;
    //////////////////////////////////////////////////////////////////////////
    enum class EMCPFrameType : uint8_t
    {
        Request = 1,
        Response = 2,
        Event = 3,
        Binary = 4,
        Cancel = 5
    };
    //////////////////////////////////////////////////////////////////////////
    enum EMCPFrameFlag : uint8_t
    {
        MCP_FRAME_FLAG_NONE = 0,
        MCP_FRAME_FLAG_JSON = 1 << 0,
        MCP_FRAME_FLAG_FINAL = 1 << 1
    };
    //////////////////////////////////////////////////////////////////////////
    struct MCPFrame
    {
        EMCPFrameType type;
        uint8_t flags;
        uint32_t requestId;
        uint32_t chunkIndex;
        uint32_t chunkCount;
        Data payload;
    };
    //////////////////////////////////////////////////////////////////////////
    class MCPFrameDecoder
    {
    public:
        MCPFrameDecoder();
        ~MCPFrameDecoder();

    public:
        bool append( const void * _buffer, size_t _size, Vector<MCPFrame> * const _frames, const Char ** const _error );
        void reset();

    protected:
        Data m_buffer;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        Data makeMCPFrame( EMCPFrameType _type, uint8_t _flags, uint32_t _requestId, uint32_t _chunkIndex, uint32_t _chunkCount, const void * _payload, size_t _payloadSize );
        Data makeMCPJSONFrame( EMCPFrameType _type, uint32_t _requestId, const void * _payload, size_t _payloadSize );
    }
    //////////////////////////////////////////////////////////////////////////
}
