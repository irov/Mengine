#pragma once

#include "DevToDebugInterface.h"

#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetButton
        : public DevToDebugWidgetButtonInterface
    {
    public:
        DevToDebugWidgetButton();
        ~DevToDebugWidgetButton() override;

    public:
        void setTitle( const String & _title ) override;
        const String & getTitle() const override;

    protected:
        String m_title;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetButton, DevToDebugWidgetInterface> DevToDebugWidgetButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}