#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "WIN32/WindowsIncluder.h"

#include "Core/ServantBase.h"

#include <mutex>

namespace Mengine
{
    class C11ThreadMutex
        : public ServantBase<ThreadMutexInterface>
    {
    public:
        C11ThreadMutex();
        ~C11ThreadMutex() override;

    public:
        void initialize( const Char * _file, uint32_t _line );

    protected:
        void lock() override;
        void unlock() override;

    protected:
        bool try_lock() override;
        
    protected:
        std::mutex m_mutex;

#ifndef NDEBUG
        const Char * m_file;
        uint32_t m_line;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<C11ThreadMutex> C11ThreadMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}