#pragma once

#include "Interface/TimepipeInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/UpdateContext.h"
#include "Kernel/DocumentHelper.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void addTimepipe( const TimepipeInterfacePtr & _timepipe, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( const UpdateContext * )> LambdaTimepipe;
        //////////////////////////////////////////////////////////////////////////
        TimepipeInterfacePtr addTimepipe( const LambdaTimepipe & _lambda, const DocumentInterfacePtr & _doc );
        void removeTimepipe( const TimepipeInterfacePtr & _timepipe );
        //////////////////////////////////////////////////////////////////////////
    }
}