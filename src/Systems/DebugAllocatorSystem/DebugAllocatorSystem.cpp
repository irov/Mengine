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
#include "Kernel/StatisticHelper.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdString.h"
#include "Config/StdLib.h"

#if !defined(MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION)
#   ifdef MENGINE_PLATFORM_WINDOWS
#       define MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION 1
#   else
#       define MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION 0
#   endif
#endif

#if MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION == 1
#   define MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_ENABLE
#endif

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
                uint8_t v = b[index];
                
                if( v == 0xEF )
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
            m_mutexReport = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

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
#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        MENGINE_ASSERTION_FATAL( ::_heapchk() == _HEAPOK, "heap corruption" );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void * DebugAllocatorSystem::malloc( size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        size_t total_size = _size + MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE;

        void * new_mem = MENGINE_MALLOC( total_size );

        MENGINE_ASSERTION_MEMORY_PANIC( new_mem, "invalid alloc memory '%zu' total '%u' [%s]"
            , total_size
            , Helper::atomicLoad( m_memoryUsage )
            , _doc
        );

#if defined(MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_ENABLE)
        Detail::setMemoryOverrideCorruptionTrap( new_mem, _size );
#endif

        size_t usage_size = MENGINE_MALLOC_SIZE( new_mem );

        MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1, "invalid get memory size" );

        this->report( _doc, usage_size, 0 );

        STATISTIC_ADD_INTEGER( STATISTIC_ALLOCATOR_NEW, usage_size );
        STATISTIC_ADD_INTEGER( STATISTIC_ALLOCATOR_SIZE, usage_size );

        return new_mem;
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

        size_t old_size = MENGINE_MALLOC_SIZE( _mem );

        MENGINE_ASSERTION_FATAL( old_size != (size_t)-1, "invalid get memory size" );

#if defined(MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_ENABLE)
        if( Detail::checkMemoryOverrideCorruptionTrap( _mem, old_size ) == true )
        {
            MENGINE_ERROR_FATAL( "memory corruption [%p] %s"
                , _mem
                , _doc
            );
        }
#endif

        MENGINE_FREE( _mem );

        this->report( _doc, 0, old_size );

        STATISTIC_ADD_INTEGER( STATISTIC_ALLOCATOR_FREE, old_size );
        STATISTIC_DEL_INTEGER( STATISTIC_ALLOCATOR_SIZE, old_size );
    }
    //////////////////////////////////////////////////////////////////////////
    void * DebugAllocatorSystem::calloc( size_t _num, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        size_t calloc_size = _num * _size;
        size_t total_size = calloc_size + MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE;

        void * new_mem = MENGINE_MALLOC( total_size );

        MENGINE_ASSERTION_MEMORY_PANIC( new_mem, "invalid calloc memory '%zu' total '%u' [%s]"
            , total_size
            , Helper::atomicLoad( m_memoryUsage )
            , _doc
        );

        MENGINE_MEMSET( new_mem, 0x00, calloc_size );

#if defined(MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_ENABLE)
        Detail::setMemoryOverrideCorruptionTrap( new_mem, calloc_size );
#endif

        size_t usage_size = MENGINE_MALLOC_SIZE( new_mem );

        MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1, "invalid get memory size" );

        this->report( _doc, usage_size, 0 );

        STATISTIC_ADD_INTEGER( STATISTIC_ALLOCATOR_NEW, usage_size );
        STATISTIC_ADD_INTEGER( STATISTIC_ALLOCATOR_SIZE, usage_size );

        return new_mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void * DebugAllocatorSystem::realloc( void * _mem, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        size_t total_size = _size + MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE;

        if( _mem == nullptr )
        {
            void * mem = MENGINE_MALLOC( total_size );

            MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' total '%u' from nullptr [%s]"
                , total_size
                , Helper::atomicLoad( m_memoryUsage )
                , _doc
            );

#if defined(MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_ENABLE)
            Detail::setMemoryOverrideCorruptionTrap( mem, _size );
#endif

            size_t usage_size = MENGINE_MALLOC_SIZE( mem );

            MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1, "invalid get memory size" );

            this->report( _doc, usage_size, 0 );

            STATISTIC_ADD_INTEGER( STATISTIC_ALLOCATOR_NEW, usage_size );
            STATISTIC_ADD_INTEGER( STATISTIC_ALLOCATOR_SIZE, usage_size );

            return mem;
        }
 
        size_t old_size = MENGINE_MALLOC_SIZE( _mem );

        MENGINE_ASSERTION_FATAL( old_size != (size_t)-1, "invalid get memory size" );

#if defined(MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_ENABLE)
        if( Detail::checkMemoryOverrideCorruptionTrap( _mem, old_size ) == true )
        {
            MENGINE_ERROR_FATAL( "memory corruption: %s"
                , _doc
            );
        }
#endif

        void * new_mem = MENGINE_REALLOC( _mem, total_size );

        MENGINE_ASSERTION_MEMORY_PANIC( new_mem, "invalid realloc memory '%zu' total '%u' from '%p' [%s]"
            , total_size
            , Helper::atomicLoad( m_memoryUsage )
            , _mem
            , _doc
        );

#if defined(MENGINE_DEBUG_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_ENABLE)
        Detail::setMemoryOverrideCorruptionTrap( new_mem, _size );
#endif

        size_t usage_size = MENGINE_MALLOC_SIZE( new_mem );

        MENGINE_ASSERTION_FATAL( usage_size != (size_t)-1, "invalid get memory size" );

        this->report( _doc, usage_size, old_size );

        STATISTIC_ADD_INTEGER( STATISTIC_ALLOCATOR_NEW, usage_size );
        STATISTIC_ADD_INTEGER( STATISTIC_ALLOCATOR_FREE, old_size );

        STATISTIC_ADD_INTEGER( STATISTIC_ALLOCATOR_SIZE, usage_size );
        STATISTIC_DEL_INTEGER( STATISTIC_ALLOCATOR_SIZE, old_size );
        
        return new_mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugAllocatorSystem::beginThread( ThreadId _threadId )
    {
        MENGINE_UNUSED( _threadId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugAllocatorSystem::endThread( ThreadId _threadId )
    {
        MENGINE_UNUSED( _threadId );

        //Empty
    }
    ////////////////////////////////////////////////////////////////////////
    void DebugAllocatorSystem::report( const Char * _doc, size_t _add, size_t _minus )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexReport );

        uint32_t report_add = (uint32_t)_add;
        uint32_t report_minus = (uint32_t)_minus;

        MENGINE_ASSERTION_FATAL( m_memoryUsage + report_add >= report_minus, "invalid memory usage" );

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

            MENGINE_ASSERTION_FATAL( r.block == false, "doc '%s' block"
                , _doc
            );

            MENGINE_ASSERTION_FATAL( r.count + report_add >= report_minus, "doc '%s' invalid report"
                , _doc
            );

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

            MENGINE_ASSERTION_FATAL( r.block == false, "doc '%s' block"
                , _doc
            );

            MENGINE_ASSERTION_FATAL( r.count + report_add >= report_minus, "doc '%s' invalid report"
                , _doc
            );

            r.doc.assign( _doc );

            r.count += report_add;
            r.count -= report_minus;

            break;
        }
    }
    ////////////////////////////////////////////////////////////////////////
    void DebugAllocatorSystem::blockAllocator( const Char * _doc )
    {
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

            r.block = true;

            return;
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
