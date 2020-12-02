#include "RandomDevice.h"

#include <chrono>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        uint32_t generateRandomDeviceSeed()
        {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
            std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>::duration epoch = now_ns.time_since_epoch();
            std::chrono::nanoseconds value = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);

            uint32_t now_ns_count = (uint32_t)value.count();

            return now_ns_count;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}