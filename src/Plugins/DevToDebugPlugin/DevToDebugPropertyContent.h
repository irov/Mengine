#pragma once

#include "DevToDebugProperty.h"

#include "Kernel/String.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugPropertyContent
        : public DevToDebugProperty
        , UnknownDevToDebugPropertyContentInterface
    {
        DECLARE_FACTORABLE( DevToDebugPropertyContent );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugPropertyContent();
        ~DevToDebugPropertyContent() override;

    public:
        void addState( const ConstString & _id, const String & _text ) override;

    public:
        bool fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const override;

    protected:
        mutable bool m_invalidate;

        struct StateDesc
        {
            ConstString id;
            String text;
        };

        typedef Vector<StateDesc> VectorStateDescs;
        VectorStateDescs m_states;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugPropertyContent, DevToDebugPropertyInterface> DevToDebugPropertyContentPtr;
    //////////////////////////////////////////////////////////////////////////
}