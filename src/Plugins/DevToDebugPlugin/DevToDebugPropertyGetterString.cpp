#include "DevToDebugPropertyGetterString.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyGetterString::DevToDebugPropertyGetterString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyGetterString::~DevToDebugPropertyGetterString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyGetterString::setGetter( const LambdaGetterValue & _getter )
    {
        m_getter = _getter;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyGetterString::initialize()
    {
        m_mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyGetterString::finalize()
    {
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyGetterString::sync()
    {
        String value;
        m_getter( &value );

        m_mutex->lock();
        m_cache = value;
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyGetterString::fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const
    {
        m_mutex->lock();
        if( _force == false )
        {
            if( m_test == m_cache )
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