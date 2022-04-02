#pragma once

#include "DevToDebugInterface.h"

#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetText
        : public DevToDebugWidgetTextInterface
    {
    public:
        DevToDebugWidgetText();
        ~DevToDebugWidgetText() override;

    public:
        void setConstText( const String & _text ) override;
        virtual void setGetterTitle( const LambdaGetterText & _getter ) override;

        const String & calculateText() const override;

    protected:
        mutable String m_text;
        LambdaGetterText m_getter;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetText, DevToDebugWidgetInterface> DevToDebugWidgetTextPtr;
    //////////////////////////////////////////////////////////////////////////
}