#pragma once

#include "Kernel/UpdateContext.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        typedef Lambda<void( const UpdateContext * )> LambdaTimepipe;
        uint32_t addTimepipe( const LambdaTimepipe & _lambda );
        void removeTimepipe( uint32_t _id );
    }
}