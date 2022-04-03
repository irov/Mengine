#pragma once

#include "DevToDebugInterface.h"

#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugTab
        : public DevToDebugTabInterface
    {
    public:
        DevToDebugTab();
        ~DevToDebugTab() override;

    public:
        void addWidget( const DevToDebugWidgetInterfacePtr & _widget ) override;
        void foreachWidgets( const LambdaForeachWidgets & _lambda ) override;

    protected:
        typedef Vector<DevToDebugWidgetInterfacePtr> VectorDevToDebugWidgets;
        VectorDevToDebugWidgets m_widgets;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugTab, DevToDebugTabInterface> DevToDebugTabPtr;
    //////////////////////////////////////////////////////////////////////////
}