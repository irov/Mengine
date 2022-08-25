#pragma once

#include "DevToDebugWidget.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetButton
        : public DevToDebugWidget
        , public UnknownDevToDebugWidgetButtonInterface
    {
        DECLARE_FACTORABLE( DevToDebugWidgetButton );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugWidgetButton();
        ~DevToDebugWidgetButton() override;

    public:
        void setClickEvent( const LambdaClickEvent & _clickEvent ) override;

    protected:
        const Char * getTypeJson() const override;

    protected:
        void process( const jpp::object & _data, const ThreadMutexInterfacePtr & _mutex, VectorDevToDebugWidgetCommands * const _commands ) override;

    protected:
        LambdaClickEvent m_clickEvent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetButton, DevToDebugWidgetInterface> DevToDebugWidgetButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}