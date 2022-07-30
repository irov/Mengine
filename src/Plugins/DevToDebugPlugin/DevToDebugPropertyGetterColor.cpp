#include "DevToDebugPropertyGetterColor.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/DocumentHelper.h"

#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyGetterColor::DevToDebugPropertyGetterColor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyGetterColor::~DevToDebugPropertyGetterColor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyGetterColor::setGetter( const LambdaGetterValue & _getter )
    {
        m_getter = _getter;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyGetterColor::initialize()
    {
        m_mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyGetterColor::finalize()
    {
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyGetterColor::sync()
    {
        Color value;
        m_getter( &value );

        m_mutex->lock();
        m_cache = value;
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyGetterColor::fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const
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

        ColorValue_ARGB argb = m_test.getAsARGB();

        Char hex_value[16] = {'\0'};
        MENGINE_SPRINTF( hex_value, "#%08X"
            , argb
        );

        _jwidget.set( _name.c_str(), hex_value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}