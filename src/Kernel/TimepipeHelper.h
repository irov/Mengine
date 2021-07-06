#pragma once

#include "Kernel/UpdateContext.h"
#include "Kernel/Document.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    namespace Helper
    {
        typedef Lambda<void( const UpdateContext * )> LambdaTimepipe;
        UniqueId addTimepipe( const LambdaTimepipe & _lambda, const DocumentPtr & _doc );
        void removeTimepipe( UniqueId _id );
    }
}