#pragma once

#include "DevToDebugWidget.h"

#include "Kernel/String.h"
#include "Kernel/Unknowable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetText
        : public DevToDebugWidget
        , public UnknownDevToDebugWidgetTextInterface
    {
        DECLARE_FACTORABLE( DevToDebugWidgetText );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugWidgetText();
        ~DevToDebugWidgetText() override;

    protected:
        const Char * getTypeJson() const override;

    protected:
        void process( const jpp::object & _data, const ThreadMutexInterfacePtr & _mutex, VectorDevToDebugWidgetCommands * const _commands ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetText, DevToDebugWidgetInterface> DevToDebugWidgetTextPtr;
    //////////////////////////////////////////////////////////////////////////
}