#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TransformationInterface;
    //////////////////////////////////////////////////////////////////////////
    class Transformable
        : public Mixin
    {
    public:
        virtual TransformationInterface * getTransformation()
        {
            return nullptr;
        }

        virtual const TransformationInterface * getTransformation() const
        {
            return nullptr;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Transformable> TransformablePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_TRANSFORMABLE()\
public:\
    Mengine::TransformationInterface * getTransformation() override{ return this; }\
    const Mengine::TransformationInterface * getTransformation() const override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////
