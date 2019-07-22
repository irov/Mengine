#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseEventation.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EScriptHolderEventFlag
    {
        EVENT_KEEP_SCRIPT = 0,
        EVENT_RELEASE_SCRIPT
    };
    //////////////////////////////////////////////////////////////////////////
    class ScriptHolderEventReceiver
        : public EventReceiverInterface
    {
    public:
        virtual pybind::object onScriptHolderKeepScript() = 0;
        virtual void onScriptHolderReleaseScript( const pybind::object & _script ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptHolderEventReceiver> ScriptHolderEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    class ScriptHolder
        : public Node
        , public BaseEventation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_EVENTABLE( ScriptHolderEventReceiver );

    public:
        ScriptHolder();

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        pybind::object m_script;
    };
}
