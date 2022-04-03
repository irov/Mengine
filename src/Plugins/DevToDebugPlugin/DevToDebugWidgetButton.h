#pragma once

#include "DevToDebugWidget.h"

#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetButton
        : public DevToDebugWidget
        , public DevToDebugWidgetButtonInterface
    {
    public:
        DevToDebugWidgetButton();
        ~DevToDebugWidgetButton() override;

    public:
        void setTitle( const String & _title ) override;

    protected:
        void _fillDataJson( jpp::object & _jdata ) override;

    protected:
        String m_title;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetButton, DevToDebugWidgetInterface> DevToDebugWidgetButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}