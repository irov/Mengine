#include "DevToDebugWidgetCommandLine.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetCommandLine::DevToDebugWidgetCommandLine()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetCommandLine::~DevToDebugWidgetCommandLine()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetCommandLine::setCommandEvent( const LambdaCommandEvent & _commandEvent )
    {
        m_commandEvent = _commandEvent;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * DevToDebugWidgetCommandLine::getTypeJson() const
    {
        return "command-line";
    }
    ////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetCommandLine::process( const jpp::object & _data, const ThreadMutexInterfacePtr & _mutex, VectorDevToDebugWidgetCommands * const _commands )
    {
        MENGINE_UNUSED( _data );

        if( m_commandEvent == nullptr )
        {
            return;
        }

        const Char * value = _data.get( "value" );

        m_commandAux = value;

        _mutex->lock();
        _commands->emplace_back( [this]()
        {
            m_commandEvent( m_commandAux );
        } );
        _mutex->unlock();
    }
    ////////////////////////////////////////////////////////////////////////
}