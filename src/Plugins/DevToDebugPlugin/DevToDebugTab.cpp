#include "DevToDebugTab.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugTab::DevToDebugTab()
        : m_invalidateWidgets( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugTab::~DevToDebugTab()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugTab::initialize()
    {
        m_mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugTab::finalize()
    {
        m_mutex = nullptr;

        m_widgets.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugTab::addWidget( const DevToDebugWidgetInterfacePtr & _widget )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _widget );
        
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_widgets.emplace_back( _widget );

        m_invalidateWidgets = true;
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
    void DevToDebugTab::syncWidgets()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const DevToDebugWidgetInterfacePtr & widget : m_widgets )
        {
            widget->syncProperties();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugTab::foreachWidgets( const LambdaForeachWidgets & _lambda, bool * const _invalidate )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const DevToDebugWidgetInterfacePtr & widget : m_widgets )
        {
            _lambda( widget );
        }

        *_invalidate = m_invalidateWidgets;

        m_invalidateWidgets = false;
    }
    //////////////////////////////////////////////////////////////////////////
}
