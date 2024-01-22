#include "WriteHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void writeUint32( void * _buffer, uint32_t _value )
        {
            uint8_t * buffer = reinterpret_cast<uint8_t *>(_buffer);

            buffer[0] = (_value >> 0) & 0xFF;
            buffer[1] = (_value >> 8) & 0xFF;
            buffer[2] = (_value >> 16) & 0xFF;
            buffer[3] = (_value >> 24) & 0xFF;
        }
        //////////////////////////////////////////////////////////////////////////
        void writeUint64( void * _buffer, uint64_t _value )
        {
            uint8_t * buffer = reinterpret_cast<uint8_t *>(_buffer);

            buffer[0] = (_value >> 0) & 0xFF;
            buffer[1] = (_value >> 8) & 0xFF;
            buffer[2] = (_value >> 16) & 0xFF;
            buffer[3] = (_value >> 24) & 0xFF;
            buffer[4] = (_value >> 32) & 0xFF;
            buffer[5] = (_value >> 40) & 0xFF;
            buffer[6] = (_value >> 48) & 0xFF;
            buffer[7] = (_value >> 56) & 0xFF;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}