#include "DevToDebugTab.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugTab::DevToDebugTab()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugTab::~DevToDebugTab()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugTab::addWidget( const DevToDebugWidgetInterfacePtr & _widget )
    {
        m_widgets.emplace_back( _widget );
    }
    //////////////////////////////////////////////////////////////////////////
    const DevToDebugWidgetInterfacePtr & DevToDebugTab::findWidget( const ConstString & _id ) const
    {
        for( const DevToDebugWidgetInterfacePtr & widget : m_widgets )
        {
            const ConstString & id = widget->getId();

            if( id != _id )
            {
                continue;
            }

            return widget;
        }

        return DevToDebugWidgetInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugTab::foreachWidgets( const LambdaForeachWidgets & _lambda )
    {
        for( const DevToDebugWidgetInterfacePtr & widget : m_widgets )
        {
            _lambda( widget );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
