#pragma once

#include "DevToDebugWidget.h"

#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetText
        : public DevToDebugWidget
        , public DevToDebugWidgetTextInterface
    {
    public:
        DevToDebugWidgetText();
        ~DevToDebugWidgetText() override;

    public:
        void setConstText( const String & _text ) override;
        virtual void setGetterTitle( const LambdaGetterText & _getter ) override;

    protected:
        const String & calculateText() const;

    protected:
        void _fillDataJson( jpp::object & _jdata ) override;

    protected:
        mutable String m_text;
        LambdaGetterText m_getter;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetText, DevToDebugWidgetInterface> DevToDebugWidgetTextPtr;
    //////////////////////////////////////////////////////////////////////////
}