#include "UID.h"

#include <random>
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
            std::default_random_engine rng( std::random_device{}() );

            std::uniform_int_distribution<uint32_t> dist( 0, sizeof( char_array ) - 1 );

            std::generate_n( _uid, _length, [&dist, &rng]()
            {
                uint32_t e = dist( rng ); 
                
                return char_array[e];
            } );
        }
    }
}