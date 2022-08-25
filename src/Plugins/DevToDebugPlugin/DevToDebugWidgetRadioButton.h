#pragma once

#include "DevToDebugWidget.h"

#include "Kernel/String.h"
#include "Kernel/Unknowable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetRadioButton
        : public DevToDebugWidget
        , public UnknownDevToDebugWidgetRadioButtonInterface
    {
        DECLARE_FACTORABLE( DevToDebugWidgetRadioButton );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugWidgetRadioButton();
        ~DevToDebugWidgetRadioButton() override;

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
    typedef IntrusivePtr<DevToDebugWidgetRadioButton, DevToDebugWidgetInterface> DevToDebugWidgetRadioButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}