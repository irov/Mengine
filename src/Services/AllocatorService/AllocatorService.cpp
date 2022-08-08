#include "AllocatorService.h"

#include "Interface/ThreadSystemInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Error.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_ALLOCATOR_RPMALLOC
#   ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
#       define MENGINE_ALLOCATOR_RPMALLOC 0
#   else
#       define MENGINE_ALLOCATOR_RPMALLOC 1
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
#if MENGINE_ALLOCATOR_RPMALLOC == 1
#   define MENGINE_ALLOCATOR_RPMALLOC_ENABLE
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_ALLOCATOR_RPMALLOC_ENABLE
#   define ENABLE_PRELOAD 1
extern "C" {
#   include "rpmalloc/rpmalloc.h"
}
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
#   include "Kernel/ThreadMutexScope.h"
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
#   ifndef MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE
#       define MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE 128
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AllocatorService, Mengine::AllocatorService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
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
#endif
        //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_ALLOCATOR_RPMALLOC_ENABLE
        //////////////////////////////////////////////////////////////////////////
        static void rpmalloc_error_callback( const char * _message )
        {
            if( SERVICE_IS_INITIALIZE( LoggerServiceInterface ) == false )
            {
                return;
            }

            LOGGER_VERBOSE_LEVEL( Mengine::ConstString::none(), LM_ERROR, LFILTER_NONE, LCOLOR_RED, nullptr, 0 )("[rpmalloc] %s"
                , _message
                );
        }
        //////////////////////////////////////////////////////////////////////////
#endif        
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AllocatorService::AllocatorService()
#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
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
#ifdef MENGINE_ALLOCATOR_RPMALLOC_ENABLE
        rpmalloc_config_t config = {0};
        config.error_callback = &Detail::rpmalloc_error_callback;

        if( rpmalloc_initialize_config( &config ) != 0 )
        {
            return false;
        }
#endif

#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
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
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::_finalizeService()
    {
#ifdef MENGINE_ALLOCATOR_RPMALLOC_ENABLE
        rpmalloc_finalize();
#endif

#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
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

#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        void * p = ::malloc( _size + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );

        MENGINE_ASSERTION_MEMORY_PANIC( p, "invalid alloc memory '%zu' total '%zu' [%s]"
            , _size
            , (uint32_t)m_reportTotal
            , _doc
        );

        Detail::setMemoryOverrideCorruptionTrap( p, _size );

        this->report( _doc, _size + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE, 0 );
#else
#   ifdef MENGINE_ALLOCATOR_RPMALLOC_ENABLE
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

#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
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
#   ifdef MENGINE_ALLOCATOR_RPMALLOC_ENABLE
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

#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        size_t total = _num * _size;

        void * p = ::malloc( total + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );

        MENGINE_ASSERTION_MEMORY_PANIC( p, "invalid calloc memory '%zu' total '%u' [%s]"
            , _size
            , (uint32_t)m_reportTotal
            , _doc
        );

        MENGINE_MEMSET( p, 0x00, total );

        Detail::setMemoryOverrideCorruptionTrap( p, total );

        this->report( _doc, total + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE, 0 );
#else
#   ifdef MENGINE_ALLOCATOR_RPMALLOC_ENABLE
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

#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
        //MENGINE_ASSERTION_FATAL( _heapchk() == _HEAPOK );

        void * p = nullptr;

        if( _mem == nullptr )
        {
            p = ::realloc( nullptr, _size + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE );

            MENGINE_ASSERTION_MEMORY_PANIC( p, "invalid realloc memory '%zu' total '%u' from nullptr [%s]"
                , _size
                , (uint32_t)m_reportTotal
                , _doc
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

            MENGINE_ASSERTION_MEMORY_PANIC( p, "invalid realloc memory '%zu' total '%u' from '%p' [%s]"
                , _size
                , (uint32_t)m_reportTotal
                , _mem
                , _doc
            );

            Detail::setMemoryOverrideCorruptionTrap( p, _size );

            this->report( _doc, _size + MENGINE_ALLOCATOR_MEMORY_OVERRIDE_CORRUPTION_SIZE, old_size );
        }
#else
#   ifdef MENGINE_ALLOCATOR_RPMALLOC_ENABLE
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
#ifdef MENGINE_ALLOCATOR_RPMALLOC_ENABLE
          rpmalloc_thread_initialize();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::stopThread()
    {
#ifdef MENGINE_ALLOCATOR_RPMALLOC_ENABLE
        rpmalloc_thread_finalize( 1 );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool AllocatorService::checkOverrideCorruption( void * _pointer ) const
    {
        MENGINE_UNUSED( _pointer );

#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
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
    uint32_t AllocatorService::get_report_count() const
    {
#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
        return MENGINE_ALLOCATOR_REPORT_COUNT;
#else
        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AllocatorService::get_report_info( uint32_t _index, const Char ** _doc ) const
    {
        MENGINE_UNUSED( _index );

#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
        const ReportDesc & r = m_reports[_index];

        *_doc = r.doc.c_str();

        return r.count;
#else
        * _doc = "";

        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AllocatorService::get_report_total() const
    {
#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
        return m_reportTotal;
#else
        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AllocatorService::find_alloc_count( const Char * _doc ) const
    {
        MENGINE_UNUSED( _doc );

#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
        if( _doc == nullptr )
        {
            return 0;
        }

        for( uint32_t index = 0; index != MENGINE_ALLOCATOR_REPORT_COUNT; ++index )
        {
            const ReportDesc & r = m_reports[index];

            if( r.doc.compare( _doc ) != 0 )
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
#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
    void AllocatorService::report( const Char * _doc, size_t _add, size_t _minus )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexReport );

        uint32_t report_add = (uint32_t)_add;
        uint32_t report_minus = (uint32_t)_minus;

        MENGINE_ASSERTION_FATAL( m_reportTotal + report_add >= report_minus );

        m_reportTotal += report_add;
        m_reportTotal -= report_minus;

        if( _doc == nullptr )
        {
            return;
        }

        for( uint32_t index = 0; index != MENGINE_ALLOCATOR_REPORT_COUNT; ++index )
        {
            ReportDesc & r = m_reports[index];

            if( r.doc.compare( _doc ) != 0 )
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

            r.doc.assign( _doc );

            r.count += report_add;
            r.count -= report_minus;

            r.total_alloc += report_add;

            break;
        }
    }
#endif
    //////////////////////////////////////////////////////////////////////////
}
