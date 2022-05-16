#include "DevToDebugTab.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexScope.h"

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
    bool DevToDebugTab::initialize()
    {
        m_mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugTab::finalize()
    {
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugTab::addWidget( const DevToDebugWidgetInterfacePtr & _widget )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_widgets.emplace_back( _widget );
    }
    //////////////////////////////////////////////////////////////////////////
    const DevToDebugWidgetInterfacePtr & DevToDebugTab::findWidget( const ConstString & _id ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

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
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const DevToDebugWidgetInterfacePtr & widget : m_widgets )
        {
            _lambda( widget );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
