#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/Pointer.h"

#include "Config/Config.h"
#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DataInterface
        : public ServantInterface
    {
    public:
        virtual bool acquire() = 0;
        virtual void release() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DataInterface> DataInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}
