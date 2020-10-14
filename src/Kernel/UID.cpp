#include "UID.h"

#include <random>
#include <chrono>
#include <functional>
#include <algorithm>

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
            auto now = std::chrono::system_clock::now();
            auto now_ms = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
            auto epoch = now_ms.time_since_epoch();
            auto value = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);

            uint32_t now_ms_count = (uint32_t)value.count();

            std::default_random_engine rng{now_ms_count};

            std::uniform_int_distribution<uint32_t> dist( 0, sizeof( char_array ) - 1 );

            std::generate_n( _uid, _length, [&dist, &rng]()
            {
                uint32_t e = dist( rng );

                return char_array[e];
            } );
        }
    }
}