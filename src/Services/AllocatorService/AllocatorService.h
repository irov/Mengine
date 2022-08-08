#pragma once

#include "Interface/AllocatorServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/StaticString.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_ALLOCATOR_DEBUG
#   ifdef MENGINE_WINDOWS_DEBUG
#       define MENGINE_ALLOCATOR_DEBUG 1
#   else
#       define MENGINE_ALLOCATOR_DEBUG 0
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
#if MENGINE_ALLOCATOR_DEBUG == 1
#   define MENGINE_ALLOCATOR_DEBUG_ENABLE
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
#   ifndef MENGINE_ALLOCATOR_REPORT_COUNT
#   define MENGINE_ALLOCATOR_REPORT_COUNT 2048
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
#   include "Interface/ThreadMutexInterface.h"

#   include "Config/Atomic.h"
#endif
//////////////////////////////////////////////////////////////////////////
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
        void startThread() override;
        void stopThread() override;

    protected:
        bool checkOverrideCorruption( void * _pointer ) const override;

    protected:
        uint32_t get_report_count() const override;
        size_t get_report_info( uint32_t _index, const char ** _doc ) const override;
        size_t get_report_total() const override;

    protected:
        size_t find_alloc_count( const Char * _doc ) const override;

    protected:
        void report( const Char * _name, size_t _add, size_t _minus );

    protected:
#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
        ThreadMutexInterfacePtr m_mutexReport;

        struct ReportDesc
        {
            StaticString<2048> doc;
            AtomicUInt32 count = 0;
            AtomicUInt32 total_alloc = 0;
        };

        ReportDesc m_reports[MENGINE_ALLOCATOR_REPORT_COUNT];

        AtomicUInt32 m_reportTotal;
#endif
    };
}
