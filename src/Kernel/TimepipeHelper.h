#pragma once

#include "Interface/DocumentInterface.h"

#include "Kernel/UpdateContext.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    namespace Helper
    {
        typedef Lambda<void( const UpdateContext * )> LambdaTimepipe;
        UniqueId addTimepipe( const LambdaTimepipe & _lambda, const DocumentInterfacePtr & _doc );
        void removeTimepipe( UniqueId _id );
    }
}