#pragma once

#include "DevToDebugInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidget
        : virtual public DevToDebugWidgetInterface
    {
    public:
        DevToDebugWidget();
        ~DevToDebugWidget() override;

    public:
        void setId( const ConstString & _id ) override;
        const ConstString & getId() const override;

        void setHide( bool _hide ) override;
        bool getHide() const override;

    protected:
        void fillJson( jpp::object & _jwidget ) override;

    protected:
        virtual void _fillDataJson( jpp::object & _jdata ) = 0;

    protected:
        ConstString m_id;
        bool m_hide;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidget, DevToDebugWidgetInterface> DevToDebugWidgetPtr;
    //////////////////////////////////////////////////////////////////////////
}