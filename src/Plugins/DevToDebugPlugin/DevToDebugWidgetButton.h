#pragma once

#include "DevToDebugWidget.h"

#include "Kernel/String.h"
#include "Kernel/Unknowable.h"

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
        void setTitle( const String & _title ) override;
        const String & getTitle() const override;

    public:
        void setClickEvent( const LambdaClickEvent & _clickEvent ) override;

    protected:
        void _fillTypeJson( jpp::object & _jdata ) override;
        void _fillDataJson( jpp::object & _jdata ) override;

    protected:
        void process( const jpp::object & _data ) override;

    protected:
        bool _checkInvalidate() const override;

    protected:
        String m_title;

        LambdaClickEvent m_clickEvent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetButton> DevToDebugWidgetButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}