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
        MENGINE_ASSERTION_FATAL( ::HeapValidate( m_hHeap, 0, 0 ) == TRUE, "heap is not valid" );

        ::HeapDestroy( m_hHeap );
        m_hHeap = NULL;
    }
    //////////////////////////////////////////////////////////////////////////
    void * Win32AllocatorSystem::malloc( size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        LPVOID mem = ::HeapAlloc( m_hHeap, 0, _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid alloc memory '%zu' [%s]"
            , _size
            , _doc
        );

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

        BOOL result = ::HeapFree( m_hHeap, 0, _mem );

        MENGINE_UNUSED( result );

        MENGINE_ASSERTION_FATAL( result == TRUE, "invalid free memory [%s]"
            , _doc
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void * Win32AllocatorSystem::calloc( size_t _num, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        LPVOID mem = ::HeapAlloc( m_hHeap, 0, _num * _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid calloc memory '%zu' [%s]"
            , _num * _size
            , _doc
        );

        ::FillMemory( mem, _num * _size, 0x0 );

        return mem;
    }
    //////////////////////////////////////////////////////////////////////////
    void * Win32AllocatorSystem::realloc( void * _mem, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        if( _mem == nullptr )
        {
            LPVOID mem = ::HeapAlloc( m_hHeap, 0, _size );

            MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' from [nullptr] [%s]"
                , _size
                , _mem
                , _doc
            );

            return mem;
        }

        LPVOID mem = ::HeapReAlloc( m_hHeap, 0, _mem, _size );

        MENGINE_ASSERTION_MEMORY_PANIC( mem, "invalid realloc memory '%zu' from [%p] [%s]"
            , _size
            , _mem
            , _doc
        );

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
    //////////////////////////////////////////////////////////////////////////
}
