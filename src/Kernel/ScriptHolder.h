#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class ScriptHolder
		: public Node
	{
	public:
		ScriptHolder();

	protected:
		void _setEventListener( const pybind::dict & _listener ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		pybind::object m_script;
	};
}
