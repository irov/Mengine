#pragma once

#include "Interface/AllocatorServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"

#ifdef MENGINE_WINDOWS_DEBUG
#   define MENGINE_ALLOCATOR_DEBUG

#   include "Config/Atomic.h"
#endif

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
        uint32_t get_report_count() const override;
        size_t get_report_info( uint32_t _index, const char ** _doc ) const override;
        size_t get_report_total() const override;

    protected:
        size_t find_alloc_count( const Char * _doc ) const override;

    protected:
        void waitThread_();
        void leaveThread_();

    protected:
        void report( const Char * _name, size_t _add, size_t _minus );

    protected:
        ThreadMutexInterfacePtr m_mutexAllocatorPool;

#ifdef MENGINE_ALLOCATOR_DEBUG
        ThreadMutexInterfacePtr m_mutexReport;

        struct ReportDesc
        {
            char doc[2048] = {'\0'};
            AtomicUInt32 count;
        };

        ReportDesc m_reports[2048];

        AtomicUInt32 m_reportTotal;
#endif
    };
}
