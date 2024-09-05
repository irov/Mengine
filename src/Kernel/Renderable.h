#pragma once

#include "Interface/RenderInterface.h"

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

        virtual const RenderInterface * getRender() const
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
    const Mengine::RenderInterface * getRender() const override{ return this; }\
    Mengine::Renderable * getRenderable() override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////
