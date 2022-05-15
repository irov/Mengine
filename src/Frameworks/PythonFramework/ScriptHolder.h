#pragma once

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Node.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BaseTransformation.h"

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
        , protected BaseEventation
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( ScriptHolder );
        DECLARE_VISITABLE( Node );
        DECLARE_EVENTABLE();
        DECLARE_TRANSFORMABLE();

    public:
        ScriptHolder();
        ~ScriptHolder() override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        pybind::object m_script;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<ScriptHolder> ScriptHolderPtr;
    //////////////////////////////////////////////////////////////////////////
}
