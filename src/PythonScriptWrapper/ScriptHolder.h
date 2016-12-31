#	pragma once

#	include "Kernel/Node.h"

#   include "pybind/pybind.hpp"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    enum ScriptHolderEventFlag
    {
        EVENT_KEEP_SCRIPT = 0,
        EVENT_RELEASE_SCRIPT
    };
    //////////////////////////////////////////////////////////////////////////
    class ScriptHolderEventReceiver
        : public EventReceiver
    {
    public:
        virtual pybind::object onScriptHolderKeepScript() = 0;
        virtual void onScriptHolderReleaseScript( const pybind::object & _script ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<ScriptHolderEventReceiver> ScriptHolderEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
	class ScriptHolder
		: public Node
		, public Eventable
	{
        EVENT_RECEIVER( ScriptHolderEventReceiver );

	public:
		ScriptHolder();

	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		pybind::object m_script;
	};
}
