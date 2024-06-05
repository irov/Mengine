#pragma once

#include "Interface/AllocatorSystemInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/StaticString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    class RPAllocatorSystem
        : public ServiceBase<AllocatorSystemInterface>
    {
    public:
        RPAllocatorSystem();
        ~RPAllocatorSystem() override;

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
    };
}
