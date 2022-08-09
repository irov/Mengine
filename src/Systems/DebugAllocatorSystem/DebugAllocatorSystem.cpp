#include "DebugAllocatorSystem.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Error.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/ThreadMutexScope.h"

#include "Config/StdString.h"

#include <stdlib.h>

#ifndef MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE
#define MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE 128
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AllocatorSystem, Mengine::DebugAllocatorSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    ////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        ////////////////////////////////////////////////////////////////////////
        static void setMemoryOverrideCorruptionTrap( void * _p, size_t _size )
        {
            uint8_t * b = MENGINE_PVOID_OFFSET( _p, _size );

            MENGINE_MEMSET( b, 0xEF, MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );
        }
        ////////////////////////////////////////////////////////////////////////
        static bool checkMemoryOverrideCorruptionTrap( const void * _p, size_t _size )
        {
            const uint8_t * b = MENGINE_CPVOID_OFFSET( _p, _size - MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );

            for( size_t index = 0; index != MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE; ++index )
            {
                if( b[index] == 0xEF )
                {
                    continue;
                }

                return true;
            }

            return false;
        }
        ////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    DebugAllocatorSystem::DebugAllocatorSystem()
        : m_memoryUsage( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DebugAllocatorSystem::~DebugAllocatorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DebugAllocatorSystem::_initializeService()
    {
        SERVICE_WAIT( ThreadSystemInterface, [this]()
        {
            m_mutexReport = THREAD_SYSTEM()
                ->createMutex( MENGINE_DOCUMENT_MESSAGE( "AllocatorService" ) );

            return true;
        } );

        SERVICE_LEAVE( ThreadSystemInterface, [this]()
        {
            m_mutexReport = nullptr;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugAllocatorSystem::_finalizeService()
    {
#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_WINDOWS_UNIVERSAL)
        MENGINE_ASSERTION_FATAL( ::_heapchk() == _HEAPOK );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void * DebugAllocatorSystem::malloc( size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        void * mem = ::malloc( _size + MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid alloc memory '%zu' total '%zu' [%s]"
            , _size
            , this->getMemoryUsage()
            , _doc
        );

        Detail::setMemoryOverrideCorruptionTrap( mem, _size );

        size_t usage_size = _msize( mem );

        MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1 );

        this->report( _doc, usage_size, 0 );

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugAllocatorSystem::free( void * _mem, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        if( _mem == nullptr )
        {
            return;
        }

        size_t old_size = _msize( _mem );

        MENGINE_ASSERTION_FATAL( old_size != (size_t)-1 );

        if( Detail::checkMemoryOverrideCorruptionTrap( _mem, old_size ) == true )
        {
            MENGINE_ERROR_FATAL( "memory corruption [%p] %s"
                , _mem
                , _doc
            );
        }

        ::free( _mem );

        this->report( _doc, 0, old_size );
    }
    //////////////////////////////////////////////////////////////////////////
    void * DebugAllocatorSystem::calloc( size_t _num, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        size_t total = _num * _size;

        void * mem = ::malloc( total + MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid calloc memory '%zu' total '%u' [%s]"
            , _size
            , this->getMemoryUsage()
            , _doc
        );

        MENGINE_MEMSET( mem, 0x00, total );

        Detail::setMemoryOverrideCorruptionTrap( mem, total );

        size_t usage_size = _msize( mem );

        MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1 );

        this->report( _doc, usage_size, 0 );

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void * DebugAllocatorSystem::realloc( void * _mem, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        if( _mem == nullptr )
        {
            void * mem = ::malloc( _size + MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );

            MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' total '%u' from nullptr [%s]"
                , _size
                , this->getMemoryUsage()
                , _doc
            );

            Detail::setMemoryOverrideCorruptionTrap( mem, _size );

            size_t usage_size = _msize( mem );

            MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1 );

            this->report( _doc, usage_size, 0 );

            return mem;
        }
 
        size_t old_size = _msize( _mem );

        MENGINE_ASSERTION_FATAL( old_size != (size_t)-1 );

        if( Detail::checkMemoryOverrideCorruptionTrap( _mem, old_size ) == true )
        {
            MENGINE_ERROR_FATAL( "memory corruption: %s"
                , _doc
            );
        }

        void * mem = ::realloc( _mem, _size + MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' total '%u' from '%p' [%s]"
            , _size
            , this->getMemoryUsage()
            , _mem
            , _doc
        );

        Detail::setMemoryOverrideCorruptionTrap( mem, _size );

        size_t usage_size = _msize( mem );

        MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1 );

        this->report( _doc, usage_size, old_size );

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugAllocatorSystem::startThread()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugAllocatorSystem::stopThread()
    {
        //Empty
    }
    ////////////////////////////////////////////////////////////////////////
    uint32_t DebugAllocatorSystem::getMemoryUsage() const
    {
        uint32_t memoryUsage = m_memoryUsage;

        return memoryUsage;
    }
    ////////////////////////////////////////////////////////////////////////
    void DebugAllocatorSystem::report( const Char * _doc, size_t _add, size_t _minus )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexReport );

        uint32_t report_add = (uint32_t)_add;
        uint32_t report_minus = (uint32_t)_minus;

        MENGINE_ASSERTION_FATAL( m_memoryUsage + report_add >= report_minus );

        m_memoryUsage += report_add;
        m_memoryUsage -= report_minus;

        if( _doc == nullptr )
        {
            return;
        }

        for( uint32_t index = 0; index != MENGINE_ALLOCATOR_DEBUG_REPORT_COUNT; ++index )
        {
            ReportDesc & r = m_reports[index];

            if( r.doc.compare( _doc ) != 0 )
            {
                continue;
            }

            MENGINE_ASSERTION_FATAL( r.count + report_add >= report_minus );

            r.count += report_add;
            r.count -= report_minus;

            return;
        }

        for( uint32_t index = 0; index != MENGINE_ALLOCATOR_DEBUG_REPORT_COUNT; ++index )
        {
            ReportDesc & r = m_reports[index];

            if( r.doc[0] != '\0' )
            {
                continue;
            }

            MENGINE_ASSERTION_FATAL( r.count + report_add >= report_minus );

            r.doc.assign( _doc );

            r.count += report_add;
            r.count -= report_minus;

            break;
        }
    }
    ////////////////////////////////////////////////////////////////////////
    uint32_t DebugAllocatorSystem::getAllocatorReportCount() const
    {
        return MENGINE_ALLOCATOR_DEBUG_REPORT_COUNT;
    }
    ////////////////////////////////////////////////////////////////////////
    size_t DebugAllocatorSystem::getAllocatorReportInfo( uint32_t _index, const Char ** _doc ) const
    {
        const ReportDesc & r = m_reports[_index];

        *_doc = r.doc.c_str();

        return r.count;
    }
    ////////////////////////////////////////////////////////////////////////
    size_t DebugAllocatorSystem::findAllocatorReportInfo( const Char * _doc ) const
    {
        if( _doc == nullptr )
        {
            return 0;
        }

        for( uint32_t index = 0; index != MENGINE_ALLOCATOR_DEBUG_REPORT_COUNT; ++index )
        {
            const ReportDesc & r = m_reports[index];

            if( r.doc.compare( _doc ) != 0 )
            {
                continue;
            }

            size_t count = r.count;

            return count;
        }

        return 0;
    }
    ////////////////////////////////////////////////////////////////////////
}
