#include "C11ThreadMutex.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    C11ThreadMutex::C11ThreadMutex()
#ifndef NDEBUG
        : m_file( nullptr )
        , m_line( 0 )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    C11ThreadMutex::~C11ThreadMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void C11ThreadMutex::initialize( const Char * _file, uint32_t _line )
    {
        (void)_file;
        (void)_line;

#ifndef NDEBUG
        m_file = _file;
        m_line = _line;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void C11ThreadMutex::lock()
    {
        m_mutex.lock();
    }
    //////////////////////////////////////////////////////////////////////////
    void C11ThreadMutex::unlock()
    {
        m_mutex.unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool C11ThreadMutex::try_lock()
    {
        if( m_mutex.try_lock() == true )
        {
            return false;
        }

        return true;
    }
}