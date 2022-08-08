#include "Win32ThreadMutex.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadMutex::Win32ThreadMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadMutex::~Win32ThreadMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadMutex::initialize( const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

#ifdef MENGINE_DOCUMENT_ENABLE
        m_doc = _doc;
#endif

        ::InitializeCriticalSection( &m_cs );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::finalize()
    {
#ifdef MENGINE_DOCUMENT_ENABLE
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::lock()
    {
        ::EnterCriticalSection( &m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::unlock()
    {
        ::LeaveCriticalSection( &m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadMutex::try_lock()
    {
        if( ::TryEnterCriticalSection( &m_cs ) == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::_destroy()
    {
        ::DeleteCriticalSection( &m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
}