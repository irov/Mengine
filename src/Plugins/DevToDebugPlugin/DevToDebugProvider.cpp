#include "DevToDebugProvider.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugProvider::DevToDebugProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugProvider::~DevToDebugProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugProvider::addWidget( const DevToDebugWidgetInterfacePtr & _widget )
    {
        m_widgets.emplace_back( _widget );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugProvider::foreachWidgets( const LambdaForeachWidgets & _lambda )
    {
        for( const DevToDebugWidgetInterfacePtr & widget : m_widgets )
        {
            _lambda( widget );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
