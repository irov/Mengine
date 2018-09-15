#pragma once

#include "Interface/RenderInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Renderable
        : public Mixin
    {
    public:
        virtual RenderInterfacePtr getRender() const
        {
            return nullptr;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Renderable> RenderablePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_RENDERABLE()\
public:\
    Mengine::RenderInterfacePtr getRender() const override{ return this; }\
protected:
