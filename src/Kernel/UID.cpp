#include "UID.h"

#include "Kernel/RandomDevice.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdRandom.h"

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
            StdRandom::mt19937_64 rng = Helper::generateRandomDeviceMT19937();

            size_t char_array_size = MENGINE_STATIC_STRING_LENGTH( char_array );

            StdRandom::uniform_int_distribution<uint64_t> dist( 0, char_array_size );

            StdAlgorithm::generate_n( _uid, _length, [&dist, &rng]()
            {
                uint64_t e = dist( rng );

                return char_array[e];
            } );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}