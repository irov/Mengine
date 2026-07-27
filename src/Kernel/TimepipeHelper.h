#pragma once

#include "Interface/TimepipeInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/LambdaTimepipe.h"
#include "Kernel/UpdateContext.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void addTimepipe( const TimepipeInterfacePtr & _timepipe, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
        typedef Mengine::LambdaTimepipe::LambdaEvent LambdaTimepipeEvent;
        //////////////////////////////////////////////////////////////////////////
        TimepipeInterfacePtr addTimepipe( const LambdaTimepipeEvent & _lambda, const DocumentInterfacePtr & _doc );
        void removeTimepipe( const TimepipeInterfacePtr & _timepipe );
        //////////////////////////////////////////////////////////////////////////
    }
}
