#include "DevToDebugPropertyGetterBoolean.h"

#include "Kernel/ThreadMutexHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyGetterBoolean::DevToDebugPropertyGetterBoolean()
        : m_cache( false )
        , m_test( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyGetterBoolean::~DevToDebugPropertyGetterBoolean()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyGetterBoolean::setGetter( const LambdaGetterValue & _getter )
    {
        m_getter = _getter;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyGetterBoolean::initialize()
    {
        m_mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyGetterBoolean::finalize()
    {
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyGetterBoolean::sync()
    {
        bool value;
        m_getter( &value );

        m_mutex->lock();
        m_cache = value;
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyGetterBoolean::fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const
    {
        m_mutex->lock();
        if( _force == false )
        {
            if( m_cache == m_test )
            {
                m_mutex->unlock();

                return false;
            }
        }

        m_test = m_cache;
        m_mutex->unlock();

        _jwidget.set( _name.c_str(), m_test );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}