#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Mixin.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PickerInterface;
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
#define DECLARE_PICKERABLE()\
public:\
    Mengine::PickerInterface * getPicker() override{ return this; }\
    const Mengine::PickerInterface * getPicker() const override{ return this; }\
    Mengine::Pickerable * getPickerable() override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////