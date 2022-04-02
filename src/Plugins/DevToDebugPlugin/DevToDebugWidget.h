#pragma once

#include "DevToDebugInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidget
        : public DevToDebugWidgetInterface
    {
    public:
        DevToDebugWidget();
        ~DevToDebugWidget() override;

    public:
        void setId( const ConstString & _id ) override;
        const ConstString & getId() const override;

    protected:
        ConstString m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidget, DevToDebugWidgetInterface> DevToDebugWidgetPtr;
    //////////////////////////////////////////////////////////////////////////
}