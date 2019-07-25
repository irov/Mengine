#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseEventation.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EScriptHolderEventFlag
    {
        EVENT_SCRIPT_HOLDER_KEEP = 0,
        EVENT_SCRIPT_HOLDER_RELEASE,
        __EVENT_SCRIPT_HOLDER_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class ScriptHolderEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual pybind::object onScriptHolderKeepScript() = 0;
        virtual void onScriptHolderReleaseScript( const pybind::object & _script ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptHolderEventReceiverInterface> ScriptHolderEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( ScriptHolderEventReceiverInterface, EVENT_SCRIPT_HOLDER_KEEP );
    EVENTATION_TYPEID( ScriptHolderEventReceiverInterface, EVENT_SCRIPT_HOLDER_RELEASE );
    //////////////////////////////////////////////////////////////////////////
    class ScriptHolder
        : public Node
        , public BaseEventation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_EVENTABLE();

    public:
        ScriptHolder();
        ~ScriptHolder() override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        pybind::object m_script;
    };
}
