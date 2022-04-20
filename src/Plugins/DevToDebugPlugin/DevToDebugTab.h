#pragma once

#include "DevToDebugInterface.h"

#include "Kernel/Vector.h"
#include "Kernel/Identity.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "Kernel/Scriptable.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugTab
        : public DevToDebugTabInterface
#ifdef MENGINE_USE_SCRIPT_SERVICE
        , public Scriptable
#endif
    {
        DECLARE_FACTORABLE( DevToDebugTab );

    public:
        DevToDebugTab();
        ~DevToDebugTab() override;

    public:
        void addWidget( const DevToDebugWidgetInterfacePtr & _widget ) override;
        const DevToDebugWidgetInterfacePtr & findWidget( const ConstString & _id ) const;

    public:
        void foreachWidgets( const LambdaForeachWidgets & _lambda ) override;

    protected:
        typedef Vector<DevToDebugWidgetInterfacePtr> VectorDevToDebugWidgets;
        VectorDevToDebugWidgets m_widgets;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugTab, DevToDebugTabInterface> DevToDebugTabPtr;
    //////////////////////////////////////////////////////////////////////////
}