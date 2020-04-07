#pragma once

#include "Kernel/UpdateContext.h"
#include "Kernel/Document.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        typedef Lambda<void( const UpdateContext * )> LambdaTimepipe;
        uint32_t addTimepipe( const LambdaTimepipe & _lambda, const DocumentPtr & _doc );
        void removeTimepipe( uint32_t _id );
    }
}