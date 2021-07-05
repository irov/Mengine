#include "AllocatorService.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Error.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

#include "Config/StdString.h"

#if MENGINE_ALLOCATOR_DEBUG == 0
#   define MENGINE_ALLOCATOR_RPMALLOC 0
#else
#   ifndef MENGINE_ALLOCATOR_RPMALLOC
#   define MENGINE_ALLOCATOR_RPMALLOC 1
#   endif
#endif

#if MENGINE_ALLOCATOR_RPMALLOC
#   include "rpmalloc/rpmalloc.h"
#endif

#if MENGINE_ALLOCATOR_DEBUG
#   include "Kernel/ThreadMutexScope.h"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AllocatorService, Mengine::AllocatorService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
#if MENGINE_ALLOCATOR_DEBUG
    //////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE
#define MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE 128
#endif
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void setMemoryOverrideCorruptionTrap( void * _p, size_t _size )
        {
            uint8_t * b = MENGINE_PVOID_OFFSET( _p, _size );

            MENGINE_MEMSET( b, 0xEF, MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool checkMemoryOverrideCorruptionTrap( const void * _p, size_t _size )
        {
            const uint8_t * b = MENGINE_CPVOID_OFFSET( _p, _size - MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );

            for( size_t index = 0; index != MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE; ++index )
            {
                if( b[index] == 0xEF )
                {
                    continue;
                }

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    AllocatorService::AllocatorService()
#if MENGINE_ALLOCATOR_DEBUG
        : m_reportTotal( 0 )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AllocatorService::~AllocatorService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AllocatorService::_initializeService()
    {
#if MENGINE_ALLOCATOR_DEBUG == 0
#   if MENGINE_ALLOCATOR_RPMALLOC
        rpmalloc_initialize();
        rpmalloc_thread_initialize();
#   endif
#endif

        SERVICE_WAIT( ThreadServiceInterface, [this]()
        {
            this->waitThread_();

            return true;
        } );

        SERVICE_LEAVE( ThreadServiceInterface, [this]()
        {
            this->leaveThread_();
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::_finalizeService()
    {
#if MENGINE_ALLOCATOR_DEBUG == 0
#   if MENGINE_ALLOCATOR_RPMALLOC
        rpmalloc_thread_finalize();
        rpmalloc_finalize();
#   endif
#endif

#if MENGINE_ALLOCATOR_DEBUG
#   if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_WINDOWS_UNIVERSAL)
        MENGINE_ASSERTION_FATAL( ::_heapchk() == _HEAPOK );
#   endif

        //MENGINE_ASSERTION_FATAL( m_reportTotal == 0 );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void * AllocatorService::malloc( size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

#if MENGINE_ALLOCATOR_DEBUG
        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        void * p = ::malloc( _size + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );

        MENGINE_ASSERTION_MEMORY_PANIC( p, "invalid alloc memory '%zu'"
            , _size
        );

        Detail::setMemoryOverrideCorruptionTrap( p, _size );

        this->report( _doc, _size + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE, 0 );
#else
#   if MENGINE_ALLOCATOR_RPMALLOC
        void * p = rpmalloc( _size );
#   else
        void * p = ::malloc( _size );
#   endif
#endif

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::free( void * _mem, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

#if MENGINE_ALLOCATOR_DEBUG
        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        if( _mem == nullptr )
        {
            return;
        }

        size_t old_size = _msize( _mem );

        if( Detail::checkMemoryOverrideCorruptionTrap( _mem, old_size ) == true )
        {
            MENGINE_ERROR_FATAL( "memory corruption: %s"
                , _doc
            );
        }

        ::free( _mem );

        this->report( _doc, 0, old_size );
#else
#   if MENGINE_ALLOCATOR_RPMALLOC
        rpfree( _mem );
#   else
        ::free( _mem );
#   endif
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void * AllocatorService::calloc( size_t _num, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

#if MENGINE_ALLOCATOR_DEBUG
        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        size_t total = _num * _size;

        void * p = ::malloc( total + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );

        MENGINE_ASSERTION_MEMORY_PANIC( p, "invalid alloc memory '%zu'"
            , _size
        );

        MENGINE_MEMSET( p, 0x00, total );

        Detail::setMemoryOverrideCorruptionTrap( p, total );

        this->report( _doc, total + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE, 0 );
#else
#   if MENGINE_ALLOCATOR_RPMALLOC
        void * p = rpcalloc( _num, _size );
#   else
        void * p = ::calloc( _num, _size );
#   endif
#endif

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void * AllocatorService::realloc( void * _mem, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

#if MENGINE_ALLOCATOR_DEBUG
        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        void * p = nullptr;

        if( _mem == nullptr )
        {
            p = ::realloc( nullptr, _size + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );

            MENGINE_ASSERTION_MEMORY_PANIC( p, "invalid realloc memory '%zu' from nullptr"
                , _size
            );

            Detail::setMemoryOverrideCorruptionTrap( p, _size );

            this->report( _doc, _size + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE, 0 );
        }
        else
        {
            size_t old_size = _msize( _mem );

            if( Detail::checkMemoryOverrideCorruptionTrap( _mem, old_size ) == true )
            {
                MENGINE_ERROR_FATAL( "memory corruption: %s"
                    , _doc
                );
            }

            p = ::realloc( _mem, _size + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );

            MENGINE_ASSERTION_MEMORY_PANIC( p, "invalid realloc memory '%zu' from '%p'"
                , _size
                , _mem
            );

            Detail::setMemoryOverrideCorruptionTrap( p, _size );

            this->report( _doc, _size + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE, old_size );
        }
#else
#   if MENGINE_ALLOCATOR_RPMALLOC
        void * p = rprealloc( _mem, _size );
#   else
        void * p = ::realloc( _mem, _size );
#   endif
#endif

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::startThread()
    {
#if MENGINE_ALLOCATOR_DEBUG == 0
#   if MENGINE_ALLOCATOR_RPMALLOC
        rpmalloc_thread_initialize();
#   endif
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::stopThread()
    {
#if MENGINE_ALLOCATOR_DEBUG == 0
#   if MENGINE_ALLOCATOR_RPMALLOC
        rpmalloc_thread_finalize();
#   endif
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool AllocatorService::checkOverrideCorruption( void * _pointer ) const
    {
        MENGINE_UNUSED( _pointer );

#if MENGINE_ALLOCATOR_DEBUG
#   if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_WINDOWS_UNIVERSAL)
        MENGINE_ASSERTION_FATAL( ::_heapchk() == _HEAPOK );
#   endif

        size_t size = _msize( _pointer );

        bool result = Detail::checkMemoryOverrideCorruptionTrap( _pointer, size );

        return result;
#else
        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::waitThread_()
    {
#if MENGINE_ALLOCATOR_DEBUG
        m_mutexReport = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_MESSAGE( "AllocatorService" ) );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::leaveThread_()
    {
#if MENGINE_ALLOCATOR_DEBUG
        m_mutexReport = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AllocatorService::get_report_count() const
    {
#if MENGINE_ALLOCATOR_DEBUG
        return MENGINE_ALLOCATOR_REPORT_COUNT;
#else
        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AllocatorService::get_report_info( uint32_t _index, const char ** _doc ) const
    {
        MENGINE_UNUSED( _index );

#if MENGINE_ALLOCATOR_DEBUG
        const ReportDesc & r = m_reports[_index];

        *_doc = r.doc;

        return r.count;
#else
        * _doc = "";

        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AllocatorService::get_report_total() const
    {
#if MENGINE_ALLOCATOR_DEBUG
        return m_reportTotal;
#else
        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AllocatorService::find_alloc_count( const Char * _doc ) const
    {
        MENGINE_UNUSED( _doc );

#if MENGINE_ALLOCATOR_DEBUG
        if( _doc == nullptr )
        {
            return 0;
        }

        for( uint32_t index = 0; index != MENGINE_ALLOCATOR_REPORT_COUNT; ++index )
        {
            const ReportDesc & r = m_reports[index];

            if( MENGINE_STRCMP( r.doc, _doc ) != 0 )
            {
                continue;
            }

            size_t count = r.count;

            return count;
        }
#endif

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
#if MENGINE_ALLOCATOR_DEBUG
    void AllocatorService::report( const Char * _doc, size_t _add, size_t _minus )
    {
        if( _doc == nullptr )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexReport );

        uint32_t report_add = (uint32_t)_add;
        uint32_t report_minus = (uint32_t)_minus;


        MENGINE_ASSERTION_FATAL( m_reportTotal + report_add >= report_minus );

        m_reportTotal += report_add;
        m_reportTotal -= report_minus;

        for( uint32_t index = 0; index != MENGINE_ALLOCATOR_REPORT_COUNT; ++index )
        {
            ReportDesc & r = m_reports[index];

            if( MENGINE_STRCMP( r.doc, _doc ) != 0 )
            {
                continue;
            }

            MENGINE_ASSERTION_FATAL( r.count + report_add >= report_minus );

            r.count += report_add;
            r.count -= report_minus;

            r.total_alloc += report_add;

            return;
        }

        for( uint32_t index = 0; index != MENGINE_ALLOCATOR_REPORT_COUNT; ++index )
        {
            ReportDesc & r = m_reports[index];

            if( r.doc[0] != '\0' )
            {
                continue;
            }

            MENGINE_ASSERTION_FATAL( r.count + report_add >= report_minus );

            MENGINE_STRCPY( r.doc, _doc );

            r.count += report_add;
            r.count -= report_minus;

            r.total_alloc += report_add;

            break;
        }
    }
#endif
    //////////////////////////////////////////////////////////////////////////
}
