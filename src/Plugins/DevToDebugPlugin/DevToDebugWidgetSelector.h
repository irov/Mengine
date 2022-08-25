#pragma once

#include "DevToDebugWidget.h"

#include "Kernel/String.h"
#include "Kernel/Unknowable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetSelector
        : public DevToDebugWidget
        , public UnknownDevToDebugWidgetSelectorInterface
    {
        DECLARE_FACTORABLE( DevToDebugWidgetSelector );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugWidgetSelector();
        ~DevToDebugWidgetSelector() override;

    public:
        void setChangeEvent( const LambdaChangeEvent & _changeEvent ) override;

    protected:
        const Char * getTypeJson() const override;

    protected:
        void process( const jpp::object & _data, const ThreadMutexInterfacePtr & _mutex, VectorDevToDebugWidgetCommands * const _commands ) override;

    protected:
        LambdaChangeEvent m_changeEvent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetSelector, DevToDebugWidgetInterface> DevToDebugWidgetSelectorPtr;
    //////////////////////////////////////////////////////////////////////////
}