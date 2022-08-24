#include "DevToDebugWidgetButton.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetButton::DevToDebugWidgetButton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetButton::~DevToDebugWidgetButton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetButton::setClickEvent( const LambdaClickEvent & _clickEvent )
    {
        m_clickEvent = _clickEvent;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * DevToDebugWidgetButton::getTypeJson() const
    {
        return "button";
    }
    ////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetButton::process( const jpp::object & _data, const ThreadMutexInterfacePtr & _mutex, VectorDevToDebugWidgetCommands * const _commands )
    {
        MENGINE_UNUSED( _data );

        if( m_clickEvent == nullptr )
        {
            return;
        }

        _mutex->lock();
        _commands->emplace_back( [this]()
        {
            m_clickEvent();
        } );
        _mutex->unlock();
    }
    ////////////////////////////////////////////////////////////////////////
}