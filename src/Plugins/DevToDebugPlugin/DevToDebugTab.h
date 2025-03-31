#pragma once

#include "DevToDebugInterface.h"

#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Vector.h"
#include "Kernel/Identity.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Kernel/Scriptable.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugTab
        : public DevToDebugTabInterface
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        , public Scriptable
#endif
    {
        DECLARE_FACTORABLE( DevToDebugTab );

    public:
        DevToDebugTab();
        ~DevToDebugTab() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void addWidget( const DevToDebugWidgetInterfacePtr & _widget ) override;
        const DevToDebugWidgetInterfacePtr & findWidget( const ConstString & _id ) const override;

    public:
        void syncWidgets() override;

    public:
        void foreachWidgets( const LambdaForeachWidgets & _lambda, bool * const _invalidate ) override;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        mutable bool m_invalidateWidgets;

        typedef Vector<DevToDebugWidgetInterfacePtr> VectorDevToDebugWidgets;
        VectorDevToDebugWidgets m_widgets;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugTab, DevToDebugTabInterface> DevToDebugTabPtr;
    //////////////////////////////////////////////////////////////////////////
}