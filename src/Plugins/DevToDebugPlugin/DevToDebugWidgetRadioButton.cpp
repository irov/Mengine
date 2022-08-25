#include "DevToDebugWidgetRadioButton.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetRadioButton::DevToDebugWidgetRadioButton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetRadioButton::~DevToDebugWidgetRadioButton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetRadioButton::setChangeEvent( const LambdaChangeEvent & _changeEvent )
    {
        m_changeEvent = _changeEvent;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * DevToDebugWidgetRadioButton::getTypeJson() const
    {
        return "radio-button";
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetRadioButton::process( const jpp::object & _data, const ThreadMutexInterfacePtr & _mutex, VectorDevToDebugWidgetCommands * const _commands )
    {
        MENGINE_UNUSED( _data );

        if( m_changeEvent == nullptr )
        {
            return;
        }

        ConstString value = _data.get( "value" );

        _mutex->lock();
        _commands->emplace_back( [this, value]()
        {
            m_changeEvent( value );
        } );
        _mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
}