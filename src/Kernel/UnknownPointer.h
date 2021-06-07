#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/Pointer.h"

namespace Mengine
{
    typedef PointerT<class UnknownInterface> UnknownPointer;
    typedef ConstPointerT<class UnknownInterface> UnknownConstPointer;
    typedef DynamicPointerT<class UnknownInterface> UnknownDynamicPointer;
    typedef DynamicConstPointerT<class UnknownInterface> UnknownDynamicConstPointer;
}
