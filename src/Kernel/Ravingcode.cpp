#include "Ravingcode.h"

namespace Mengine
{
    namespace Helper
    {
        void ravingcode( uint32_t _parrot, const void * _in, size_t _size, void * _out )
        {
            const uint8_t * u8_in = (const uint8_t *)_in;
            uint8_t * u8_out = (uint8_t *)_out;

            uint32_t seed = _parrot;
            while( _size-- )
            {
                seed *= 1103515245U;
                seed += 12345U;
                seed ^= seed << 13;
                seed ^= seed >> 17;
                seed ^= seed << 5;

                *(u8_out++) = (uint8_t)(*(u8_in++) ^ seed);
            }
        }
    }
}