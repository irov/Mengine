#include "RandomDevice.h"

#include "Interface/ThreadSystemInterface.h"

#include <chrono>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        uint64_t generateRandomLocaleSeed()
        {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
            std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>::duration epoch = now_ns.time_since_epoch();
            std::chrono::nanoseconds value = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);

            uint64_t now_ns_count = (uint64_t)value.count();

            return now_ns_count;
        }
        //////////////////////////////////////////////////////////////////////////
        uint64_t generateRandomDeviceSeed()
        {
            static uint64_t seed = 0;

            if( seed == 0 )
            {
                for( uint32_t probe = 0; probe != 8; ++probe )
                {
                    uint64_t locale_seed = Helper::generateRandomLocaleSeed();

                    seed ^= locale_seed;
                }

                return seed;
            }

            return seed;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}