#pragma once

#include "Interface/ScriptEmbeddableInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class LifecycleInterface;
    //////////////////////////////////////////////////////////////////////////
    class ScriptEmbeddable
        : public Mixin
    {
    public:
        virtual ScriptEmbeddableInterface * getEmbeddable()
        {
            return nullptr;
        }

        virtual const ScriptEmbeddableInterface * getEmbeddable() const
        {
            return nullptr;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptEmbeddable> ScriptEmbeddablePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_EMBEDDABLE()\
public:\
    Mengine::ScriptEmbeddableInterface * getEmbeddable() override{ return this; }\
    const Mengine::ScriptEmbeddableInterface * getEmbeddable() const override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////
