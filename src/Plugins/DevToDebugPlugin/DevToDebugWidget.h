#pragma once

#include "DevToDebugInterface.h"

#include "Kernel/ConstString.h"

#include "Kernel/JSON.h"

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

    public:
        void fillJson( jpp::object & _jwidget );

    protected:
        virtual void _fillDataJson( jpp::object & _jdata ) = 0;

    protected:
        ConstString m_id;
        bool m_hide;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidget> DevToDebugWidgetPtr;
    //////////////////////////////////////////////////////////////////////////
}