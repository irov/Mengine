#pragma once

#include "Interface/PickerInterface.h"

#include "Kernel/Node.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        PickerInterface * getNodePickerInheritance( Node * _node );
    }
}