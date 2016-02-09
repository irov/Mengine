#	pragma once

#	include "pybind/bindable.hpp"

namespace Menge
{
	class ScriptWrapperInterface;

	class Scriptable
		: public pybind::bindable
	{	
	public:
		Scriptable();
		~Scriptable();

	public:
		void setScriptWrapper( ScriptWrapperInterface * _scriptWrapper );		
		ScriptWrapperInterface * getScriptWrapper() const;

	protected:
		PyObject * _embedded() override;

	protected:
		ScriptWrapperInterface * m_scriptWrapper;
	};
}
