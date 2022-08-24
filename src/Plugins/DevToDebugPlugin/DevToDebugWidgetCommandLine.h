#pragma once

#include "DevToDebugWidget.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetCommandLine
        : public DevToDebugWidget
        , public UnknownDevToDebugWidgetCommandLineInterface
    {
        DECLARE_FACTORABLE( DevToDebugWidgetCommandLine );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugWidgetCommandLine();
        ~DevToDebugWidgetCommandLine() override;

    public:
        void setCommandEvent( const LambdaCommandEvent & _commandEvent ) override;

    protected:
        const Char * getTypeJson() const override;

    protected:
        void process( const jpp::object & _data, const ThreadMutexInterfacePtr & _mutex, VectorDevToDebugWidgetCommands * const _commands ) override;

    protected:
        LambdaCommandEvent m_commandEvent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetCommandLine, DevToDebugWidgetInterface> DevToDebugWidgetCommandLinePtr;
    //////////////////////////////////////////////////////////////////////////
}