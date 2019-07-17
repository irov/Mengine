#pragma once

#include "Interface/PickerInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Mixin.h"

#include "Config/Vector.h"

#include <type_traits>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Pickerable
        : public Mixin
    {
    public:
        virtual PickerInterface * getPicker()
        {
            return nullptr;
        };

        virtual const PickerInterface * getPicker() const
        {
            return nullptr;
        };
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Pickerable> PickerablePtr;
    //////////////////////////////////////////////////////////////////////////    
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_PICKER()\
public:\
    Mengine::PickerInterface * getPicker() override{ return this; }\
    const Mengine::PickerInterface * getPicker() const override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////