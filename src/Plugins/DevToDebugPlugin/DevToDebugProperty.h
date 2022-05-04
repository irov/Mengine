#pragma once

#include "DevToDebugInterface.h"

#include "Kernel/JSON.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugProperty
        : public DevToDebugPropertyInterface
    {
    public:
        DevToDebugProperty();
        ~DevToDebugProperty() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void sync() override;

    public:
        virtual bool fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugProperty, DevToDebugPropertyInterface> DevToDebugPropertyPtr;
    //////////////////////////////////////////////////////////////////////////
}