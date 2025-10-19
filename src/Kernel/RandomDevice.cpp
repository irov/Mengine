#include "RandomDevice.h"

#include "Kernel/Array.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        StdRandom::mt19937_64 generateRandomDeviceMT19937()
        {
            StdRandom::random_device rd;
            Array<StdRandom::random_device::result_type, 8> data = {rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
            StdRandom::seed_seq seq( data.begin(), data.end() );

            return StdRandom::mt19937_64{seq};
        }
        //////////////////////////////////////////////////////////////////////////
    }
}