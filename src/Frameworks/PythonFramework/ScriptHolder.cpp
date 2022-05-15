#include "ScriptHolder.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/EventableHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ScriptHolder::ScriptHolder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptHolder::~ScriptHolder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptHolder::_activate()
    {
        if( Node::_activate() == false )
        {
            return false;
        }

        m_script = EVENTABLE_METHODR( EVENT_SCRIPT_HOLDER_KEEP, pybind::object() )
            ->onScriptHolderKeepScript();

        bool successful = m_script.is_valid();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptHolder::_deactivate()
    {
        Node::_deactivate();

        EVENTABLE_METHOD( EVENT_SCRIPT_HOLDER_RELEASE )
            ->onScriptHolderReleaseScript( m_script );

        m_script.reset();
    }

}
