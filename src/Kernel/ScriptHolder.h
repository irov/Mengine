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
		void _setEventListener( PyObject * _listener ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		PyObject * m_script;
	};
}
