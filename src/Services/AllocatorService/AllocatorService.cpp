#include "AllocatorService.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/DocumentHelper.h"

#include "stdex/allocator.h"
#include "stdex/allocator_report.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AllocatorService, Mengine::AllocatorService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
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
    AllocatorService::AllocatorService()
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
    }
    //////////////////////////////////////////////////////////////////////////
    void * AllocatorService::malloc( size_t _size, const Char * _doc )
    {
        void * p = stdex_malloc( _size, _doc );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::free( void * _mem, const Char * _doc )
    {
        stdex_free( _mem, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    void * AllocatorService::calloc( size_t _num, size_t _size, const Char * _doc )
    {
        void * p = stdex_calloc( _num, _size, _doc );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void * AllocatorService::realloc( void * _mem, size_t _size, const Char * _doc )
    {
        void * p = stdex_realloc( _mem, _size, _doc );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AllocatorService::count( const Char * _doc ) const
    {
        uint32_t report_count = stdex_get_allocator_report_count( _doc );

        return report_count;
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::waitThread_()
    {
        m_mutexAllocatorPool = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_MESSAGE( "AllocatorService" ) );

        stdex_allocator_initialize_threadsafe( m_mutexAllocatorPool.get()
            , (stdex_allocator_thread_lock_t)&Detail::s_stdex_thread_lock
            , (stdex_allocator_thread_unlock_t)&Detail::s_stdex_thread_unlock
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void AllocatorService::leaveThread_()
    {
        stdex_allocator_finalize_threadsafe();

        m_mutexAllocatorPool = nullptr;
    }
}
