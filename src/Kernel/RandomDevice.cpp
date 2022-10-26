#include "RandomDevice.h"

#include "Interface/ThreadSystemInterface.h"

#include <chrono>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        uint32_t generateRandomLocaleSeed()
        {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
            std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>::duration epoch = now_ns.time_since_epoch();
            std::chrono::nanoseconds value = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);

            uint32_t now_ns_count = (uint32_t)value.count();

            return now_ns_count;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t generateRandomDeviceSeed()
        {
            static uint32_t seed = 0;

            if( seed == 0 )
            {
                for( uint32_t probe = 0; probe != 8; ++probe )
                {
                    uint32_t locale_seed = Helper::generateRandomLocaleSeed();

                    THREAD_SYSTEM()
                        ->sleep( 1 );

                    seed += locale_seed;
                }

                return seed;
            }

            return seed;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}