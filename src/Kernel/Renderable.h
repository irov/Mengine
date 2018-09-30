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
        virtual RenderInterface * getRender()
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
    Mengine::RenderInterface * getRender() override{ return this; }\
protected:
