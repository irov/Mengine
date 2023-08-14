#pragma once

#include "Interface/ThreadConditionVariableInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32ThreadConditionVariable
        : public ThreadConditionVariableInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( Win32ThreadConditionVariable );

    public:
        Win32ThreadConditionVariable();
        ~Win32ThreadConditionVariable() override;

    public:
        bool initialize();
        void finalize();

    protected:
        void wait( const LambdaCondition & _condition ) override;
        void wake() override;

    protected:
        void _destroy() override;

    protected:
#ifdef MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA
        CRITICAL_SECTION m_conditionLock;
        CONDITION_VARIABLE m_conditionVariable;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadConditionVariable, ThreadConditionVariableInterface> Win32ThreadConditionVariablePtr;
    //////////////////////////////////////////////////////////////////////////
}