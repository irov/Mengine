#include "Crash.h"

#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        void crash()
        {
            volatile uint32_t * p = nullptr;

            // cppcheck-suppress nullPointer
            *p = 0xBADC0DE;
        }
    }
}