#pragma once

#include "Kernel/Mixin.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadConditionVariableInterface
        : public Mixin
    {
    public:
        typedef Lambda<bool()> LambdaCondition;
        virtual void wait( const LambdaCondition & _condition ) = 0;

    public:
        virtual void wake() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadConditionVariableInterface> ThreadConditionVariableInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}