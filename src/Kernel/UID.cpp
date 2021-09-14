#include "UID.h"

#include "Kernel/RandomDevice.h"

#include <random>
#include <functional>
#include "Config/Algorithm.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static constexpr Char char_array[] = {
            '0', '1', '2', '3', '4',
            '5', '6', '7', '8', '9',
            'A', 'B', 'C', 'D', 'E', 'F',
            'G', 'H', 'I', 'J', 'K',
            'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U',
            'V', 'W', 'X', 'Y', 'Z',
            'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k',
            'l', 'm', 'n', 'o', 'p',
            'q', 'r', 's', 't', 'u',
            'v', 'w', 'x', 'y', 'z'
        };
        //////////////////////////////////////////////////////////////////////////
        void makeUID( uint32_t _length, Char * const _uid )
        {
            uint32_t seed = Helper::generateRandomDeviceSeed();

            Helper::makeUIDSeed( seed, _length, _uid );
        }
        //////////////////////////////////////////////////////////////////////////
        void makeUIDSeed( uint32_t _seed, uint32_t _length, Char * const _uid )
        {
            std::default_random_engine rng{_seed};

            std::uniform_int_distribution<uint32_t> dist( 0, MENGINE_STATIC_STRING_LENGTH( char_array ) );

            Algorithm::generate_n( _uid, _length, [&dist, &rng]()
            {
                uint32_t e = dist( rng );

                return char_array[e];
            } );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}