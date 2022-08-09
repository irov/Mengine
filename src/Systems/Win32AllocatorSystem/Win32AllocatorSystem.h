#pragma once

#include "Interface/AllocatorSystemInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/StaticString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    class Win32AllocatorSystem
        : public ServiceBase<AllocatorSystemInterface>
    {
    public:
        Win32AllocatorSystem();
        ~Win32AllocatorSystem() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void * malloc( size_t _size, const Char * _doc ) override;
        void free( void * _mem, const Char * _doc ) override;
        void * calloc( size_t _num, size_t _size, const Char * _doc ) override;
        void * realloc( void * _mem, size_t _size, const Char * _doc ) override;

    protected:
        void startThread() override;
        void stopThread() override;

    protected:
        uint32_t getMemoryUsage() const override;

    protected:
        void report( size_t _add, size_t _minus );

    protected:
        HANDLE m_hHeap;

        AtomicUInt32 m_memoryUsage;        
    };
}
