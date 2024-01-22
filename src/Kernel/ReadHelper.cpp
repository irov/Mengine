#include "ReadHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void readUint32( const void * _buffer, uint32_t * const _value )
        {
            const uint8_t * buffer = static_cast<const uint8_t *>(_buffer);

            uint32_t value = 0;

            value |= (uint32_t)buffer[0] << 0;
            value |= (uint32_t)buffer[1] << 8;
            value |= (uint32_t)buffer[2] << 16;
            value |= (uint32_t)buffer[3] << 24;

            *_value = value;
        }
        //////////////////////////////////////////////////////////////////////////
        void readUint64( const void * _buffer, uint64_t * const _value )
        {
            const uint8_t * buffer = static_cast<const uint8_t *>(_buffer);

            uint64_t value = 0;

            value |= (uint64_t)buffer[0] << 0;
            value |= (uint64_t)buffer[1] << 8;
            value |= (uint64_t)buffer[2] << 16;
            value |= (uint64_t)buffer[3] << 24;
            value |= (uint64_t)buffer[4] << 32;
            value |= (uint64_t)buffer[5] << 40;
            value |= (uint64_t)buffer[6] << 48;
            value |= (uint64_t)buffer[7] << 56;

            *_value = value;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}