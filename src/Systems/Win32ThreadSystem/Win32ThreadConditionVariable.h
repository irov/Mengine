#pragma once

#include "Interface/ThreadConditionVariableInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"

#include "Config/Char.h"

namespace Mengine
{
    class Win32ThreadConditionVariable
        : public ThreadConditionVariableInterface
        , public Factorable
    {
    public:
        Win32ThreadConditionVariable();
        ~Win32ThreadConditionVariable() override;

    public:
        void initialize( const Char * _file, uint32_t _line );

    protected:
        void wait() override;
        void wake() override;

    protected:
        void _destroy() override;

    protected:
        CRITICAL_SECTION m_conditionLock;
        CONDITION_VARIABLE m_conditionVariable;

#ifndef NDEBUG
        const Char * m_file;
        uint32_t m_line;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadConditionVariable> Win32ThreadConditionVariablePtr;
    //////////////////////////////////////////////////////////////////////////
}