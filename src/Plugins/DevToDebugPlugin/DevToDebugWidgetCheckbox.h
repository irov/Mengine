#pragma once

#include "DevToDebugWidget.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetCheckbox
        : public DevToDebugWidget
        , public UnknownDevToDebugWidgetCheckboxInterface
    {
        DECLARE_FACTORABLE( DevToDebugWidgetCheckbox );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugWidgetCheckbox();
        ~DevToDebugWidgetCheckbox() override;

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
    typedef IntrusivePtr<DevToDebugWidgetCheckbox, DevToDebugWidgetInterface> DevToDebugWidgetCheckboxPtr;
    //////////////////////////////////////////////////////////////////////////
}