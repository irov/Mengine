#pragma once

#include "Interface/AllocatorServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AllocatorService
        : public ServiceBase<AllocatorServiceInterface>
    {
    public:
        AllocatorService();
        ~AllocatorService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void * malloc( size_t _size, const Char * _doc ) override;
        void free( void * _mem, const Char * _doc ) override;
        void * calloc( size_t _num, size_t _size, const Char * _doc ) override;
        void * realloc( void * _mem, size_t _size, const Char * _doc ) override;

    protected:
        uint32_t count( const Char * _doc ) const override;

    protected:
        void waitThread_();
        void leaveThread_();

    protected:
        ThreadMutexInterfacePtr m_mutexAllocatorPool;
    };
}
