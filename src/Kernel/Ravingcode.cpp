#include "Ravingcode.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void ravingcode( const uint64_t * _parrot, uint32_t _complexity, const void * _in, size_t _size, void * const _out)
        {
            const uint8_t * u8_in = (const uint8_t *)_in;
            uint8_t * u8_out = (uint8_t *)_out;

            uint64_t seed[64];
            
            for( uint32_t index = 0; index != _complexity; ++index )
            {
                seed[index] = _parrot[index];
            }

            while( _size-- )
            {
                uint32_t index = _size % _complexity;
            
                seed[index] *= 1103515245U;
                seed[index] += 12345U;
                seed[index] ^= seed[index] << 13;
                seed[index] ^= seed[index] >> 17;
                seed[index] ^= seed[index] << 5;

                uint8_t xor_seed = seed[index] & 0xFF;

                uint8_t out_data = *(u8_in++) ^ xor_seed;

                *(u8_out++) = out_data;
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
