#pragma once

#include "Interface/AllocatorSystemInterface.h"
#include "Interface/UnknownAllocatorDebugReportInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/StaticString.h"

#include "Config/Atomic.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_ALLOCATOR_DEBUG_REPORT_COUNT
#define MENGINE_ALLOCATOR_DEBUG_REPORT_COUNT 2048
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    class DebugAllocatorSystem
        : public ServiceBase<AllocatorSystemInterface>
        , public UnknownAllocatorDebugReportInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        DebugAllocatorSystem();
        ~DebugAllocatorSystem() override;

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
        void report( const Char * _doc, size_t _add, size_t _minus );

    protected:
        void blockAllocator( const Char * _doc ) override;

    protected:
        uint32_t getAllocatorReportCount() const override;
        size_t getAllocatorReportInfo( uint32_t _index, const Char ** _doc ) const override;

    protected:
        size_t findAllocatorReportInfo( const Char * _doc ) const override;

    protected:
        ThreadMutexInterfacePtr m_mutexReport;

        struct ReportDesc
        {
            StaticString<2048> doc;
            AtomicUInt32 count = 0;
            bool block = false;
        };

        ReportDesc m_reports[MENGINE_ALLOCATOR_DEBUG_REPORT_COUNT];

        AtomicUInt32 m_memoryUsage;
    };
}
