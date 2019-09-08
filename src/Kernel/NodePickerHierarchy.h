#pragma once

#include "Interface/PickerInterface.h"

#include "Kernel/Node.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        bool hasNodePickerInheritance( Node * _node );
        PickerInterface * getNodePickerInheritance( Node * _node, Node ** _pickerNode );
    }
}