#include "DevToDebugWidgetCheckbox.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetCheckbox::DevToDebugWidgetCheckbox()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetCheckbox::~DevToDebugWidgetCheckbox()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetCheckbox::setChangeEvent( const LambdaChangeEvent & _changeEvent )
    {
        m_changeEvent = _changeEvent;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * DevToDebugWidgetCheckbox::getTypeJson() const
    {
        return "check-box";
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetCheckbox::process( const jpp::object & _data, const ThreadMutexInterfacePtr & _mutex, VectorDevToDebugWidgetCommands * const _commands )
    {
        MENGINE_UNUSED( _data );

        if( m_changeEvent == nullptr )
        {
            return;
        }

        bool value = _data.get( "value" );

        _mutex->lock();
        _commands->emplace_back( [this, value]()
        {
            m_changeEvent( value );
        } );
        _mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
}