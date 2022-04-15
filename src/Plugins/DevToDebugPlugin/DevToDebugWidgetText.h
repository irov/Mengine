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

    public:
        void setConstText( const String & _text ) override;
        virtual void setGetterText( const LambdaGetterText & _getter ) override;

    protected:
        const String & calculateText() const;

    protected:
        void _fillTypeJson( jpp::object & _jdata ) override;
        void _fillDataJson( jpp::object & _jdata ) override;

    protected:
        void process( const jpp::object & _data ) override;

    protected:
        bool _checkInvalidate() const override;

    protected:
        mutable String m_text;
        mutable String m_test;
        LambdaGetterText m_getter;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetText> DevToDebugWidgetTextPtr;
    //////////////////////////////////////////////////////////////////////////
}