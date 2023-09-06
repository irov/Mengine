#include "Win32AllocatorSystem.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Error.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AllocatorSystem, Mengine::Win32AllocatorSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32AllocatorSystem::Win32AllocatorSystem()
        : m_hHeap( NULL )
        , m_memoryUsage( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32AllocatorSystem::~Win32AllocatorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32AllocatorSystem::_initializeService()
    {
        HANDLE hHeap = ::HeapCreate( 0, 0, 0 );

        if( hHeap == NULL )
        {
            DWORD err = ::GetLastError();

            MENGINE_UNUSED( err );

            return false;
        }

        m_hHeap = hHeap;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AllocatorSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FATAL( ::HeapValidate( m_hHeap, 0, 0 ) == TRUE );

        ::HeapDestroy( m_hHeap );
        m_hHeap = NULL;
    }
    //////////////////////////////////////////////////////////////////////////
    void * Win32AllocatorSystem::malloc( size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        LPVOID mem = ::HeapAlloc( m_hHeap, 0, _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid alloc memory '%zu' total '%u' [%s]"
            , _size
            , this->getMemoryUsage()
            , _doc
        );

#if defined(MENGINE_DEBUG)
        SIZE_T usable_size = ::HeapSize( m_hHeap, 0, mem );

        MENGINE_ASSERTION_FATAL( usable_size != (SIZE_T)-1 );

        this->report( usable_size, 0 );
#endif

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AllocatorSystem::free( void * _mem, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        if( _mem == nullptr )
        {
            return;
        }

#if defined(MENGINE_DEBUG)
        SIZE_T old_size = ::HeapSize( m_hHeap, 0, _mem );

        MENGINE_ASSERTION_FATAL( old_size != (SIZE_T)-1 );
#endif

        BOOL result = ::HeapFree( m_hHeap, 0, _mem );

        MENGINE_UNUSED( result );

        MENGINE_ASSERTION_FATAL( result == TRUE );

#if defined(MENGINE_DEBUG)
        this->report( 0, old_size );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void * Win32AllocatorSystem::calloc( size_t _num, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        LPVOID mem = ::HeapAlloc( m_hHeap, 0, _num * _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid calloc memory '%zu' total '%u' [%s]"
            , _num * _size
            , this->getMemoryUsage()
            , _doc
        );

        ::FillMemory( mem, _num * _size, 0x0 );

#if defined(MENGINE_DEBUG)
        SIZE_T usable_size = ::HeapSize( m_hHeap, 0, mem );

        MENGINE_ASSERTION_FATAL( usable_size != (SIZE_T)-1 );

        this->report( usable_size, 0 );
#endif

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void * Win32AllocatorSystem::realloc( void * _mem, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        if( _mem == nullptr )
        {
            LPVOID mem = ::HeapAlloc( m_hHeap, 0, _size );

            MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' total '%u' from [nullptr] [%s]"
                , _size
                , this->getMemoryUsage()
                , _mem
                , _doc
            );

#if defined(MENGINE_DEBUG)
            SIZE_T usable_size = ::HeapSize( m_hHeap, 0, mem );

            MENGINE_ASSERTION_FATAL( usable_size != (SIZE_T)-1 );

            this->report( usable_size, 0 );
#endif

            return mem;
        }

#if defined(MENGINE_DEBUG)
        SIZE_T old_size = ::HeapSize( m_hHeap, 0, _mem );

        MENGINE_ASSERTION_FATAL( old_size != (SIZE_T)-1 );
#endif

        LPVOID mem = ::HeapReAlloc( m_hHeap, 0, _mem, _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' total '%u' from [%p] [%s]"
            , _size
            , this->getMemoryUsage()
            , _mem
            , _doc
        );

#if defined(MENGINE_DEBUG)
        SIZE_T usable_size = ::HeapSize( m_hHeap, 0, mem );

        MENGINE_ASSERTION_FATAL( usable_size != (SIZE_T)-1 );

        this->report( usable_size, old_size );
#endif

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AllocatorSystem::beginThread( ThreadId _threadId )
    {
        MENGINE_UNUSED( _threadId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32AllocatorSystem::endThread( ThreadId _threadId )
    {
        MENGINE_UNUSED( _threadId );

        //Empty
    }
    ////////////////////////////////////////////////////////////////////////
    uint32_t Win32AllocatorSystem::getMemoryUsage() const
    {
        uint32_t memoryUsage = m_memoryUsage;

        return memoryUsage;
    }
    ////////////////////////////////////////////////////////////////////////
    void Win32AllocatorSystem::report( size_t _add, size_t _minus )
    {
        MENGINE_ASSERTION_FATAL( m_memoryUsage + _add >= _minus );

        m_memoryUsage += (uint32_t)_add;
        m_memoryUsage -= (uint32_t)_minus;
    }
    //////////////////////////////////////////////////////////////////////////
}
