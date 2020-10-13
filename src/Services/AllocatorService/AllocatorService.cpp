#include "AllocatorService.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/DocumentHelper.h"

#include "Config/StdString.h"

#include "stdex/allocator.h"

#if MENGINE_ALLOCATOR_DEBUG
#   include "Kernel/ThreadMutexScope.h"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AllocatorService, Mengine::AllocatorService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_ALLOCATOR_DEBUG
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void s_stdex_thread_lock( ThreadMutexInterface * _mutex )
        {
            _mutex->lock();
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_stdex_thread_unlock( ThreadMutexInterface * _mutex )
        {
            _mutex->unlock();
        }
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    AllocatorService::AllocatorService()
#ifdef MENGINE_ALLOCATOR_DEBUG
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
#ifdef MENGINE_ALLOCATOR_DEBUG
        //MENGINE_ASSERTION_FATAL( m_reportTotal == 0 );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void * AllocatorService::malloc( size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

#ifndef MENGINE_ALLOCATOR_DEBUG
        void * p = stdex_malloc( _size );
#else
        void * p = ::malloc( _size );

        MENGINE_ASSERTION_FATAL( _size == _msize( p ) );

        this->report( _doc, _size, 0 );
#endif

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::free( void * _mem, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

#ifndef MENGINE_ALLOCATOR_DEBUG
        stdex_free( _mem );
#else
        size_t size = _mem == nullptr ? 0 : _msize( _mem );

        ::free( _mem );

        this->report( _doc, 0, size );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void * AllocatorService::calloc( size_t _num, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

#ifndef MENGINE_ALLOCATOR_DEBUG
        void * p = stdex_calloc( _num, _size );
#else
        size_t total = _num * _size;
        void * p = ::malloc( total );
        ::memset( p, 0x00, total );
                
        this->report( _doc, total, 0 );
#endif

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void * AllocatorService::realloc( void * _mem, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

#ifndef MENGINE_ALLOCATOR_DEBUG
        void * p = stdex_realloc( _mem, _size );
#else
        size_t size = _mem == nullptr ? 0 : _msize( _mem );

        void * p = ::realloc( _mem, _size );

        this->report( _doc, _size, size );
#endif

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::waitThread_()
    {
#ifndef MENGINE_ALLOCATOR_DEBUG
        m_mutexAllocatorPool = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_MESSAGE( "AllocatorService" ) );

        stdex_allocator_initialize_threadsafe( m_mutexAllocatorPool.get()
            , (stdex_allocator_thread_lock_t)&Detail::s_stdex_thread_lock
            , (stdex_allocator_thread_unlock_t)&Detail::s_stdex_thread_unlock
        );
#else
        m_mutexReport = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_MESSAGE( "AllocatorService" ) );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::leaveThread_()
    {
#ifndef MENGINE_ALLOCATOR_DEBUG
        stdex_allocator_finalize_threadsafe();

        m_mutexAllocatorPool = nullptr;
#else
        m_mutexReport = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AllocatorService::get_report_count() const
    {
#ifdef MENGINE_ALLOCATOR_DEBUG
        return 2048;
#else
        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AllocatorService::get_report_info( uint32_t _index, const char ** _doc ) const
    {
        MENGINE_UNUSED( _index );

#ifdef MENGINE_ALLOCATOR_DEBUG
        const ReportDesc & r = m_reports[_index];

        *_doc = r.doc;

        return r.count;
#else
        *_doc = "";

        return 0;
#endif        
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AllocatorService::get_report_total() const
    {
#ifdef MENGINE_ALLOCATOR_DEBUG
        return m_reportTotal;
#else
        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AllocatorService::find_alloc_count( const Char * _doc ) const
    {
        MENGINE_UNUSED( _doc );

#ifdef MENGINE_ALLOCATOR_DEBUG
        if( _doc == nullptr )
        {
            return 0;
        }

        for( uint32_t index = 0; index != 2048; ++index )
        {
            const ReportDesc & r = m_reports[index];

            if( MENGINE_STRCMP( r.doc, _doc ) != 0 )
            {
                continue;
            }

            return r.count;
        }
#endif

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_ALLOCATOR_DEBUG
    void AllocatorService::report( const Char * _doc, size_t _add, size_t _minus )
    {
        if( _doc == nullptr )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexReport );

        MENGINE_ASSERTION_FATAL( m_reportTotal + _add >= _minus );

        m_reportTotal += _add;
        m_reportTotal -= _minus;

        for( uint32_t index = 0; index != 2048; ++index )
        {
            ReportDesc & r = m_reports[index];

            if( MENGINE_STRCMP( r.doc, _doc ) != 0 )
            {
                continue;
            }

            MENGINE_ASSERTION_FATAL( r.count + _add >= _minus );

            r.count += _add;
            r.count -= _minus;

            return;
        }

        for( uint32_t index = 0; index != 2048; ++index )
        {
            ReportDesc & r = m_reports[index];

            if( r.doc[0] != '\0' )
            {
                continue;
            }

            MENGINE_ASSERTION_FATAL( _minus <= _add );

            MENGINE_STRCPY( r.doc, _doc );

            r.count += _add;
            r.count -= _minus;

            break;
        }        
    }
#endif
    //////////////////////////////////////////////////////////////////////////
}
