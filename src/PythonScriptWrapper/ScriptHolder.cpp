#include "ScriptHolder.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ScriptHolder::ScriptHolder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScriptHolder::_activate()
    {
        if( Node::_activate() == false )
        {
            return false;
        }

        m_script = EVENTABLE_METHODR( EVENT_KEEP_SCRIPT, pybind::object() )
            ->onScriptHolderKeepScript();

        bool successful = m_script.is_valid();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScriptHolder::_deactivate()
    {
        Node::_deactivate();

        EVENTABLE_METHOD( EVENT_RELEASE_SCRIPT )
            ->onScriptHolderReleaseScript( m_script );

        m_script.reset();
    }

}
