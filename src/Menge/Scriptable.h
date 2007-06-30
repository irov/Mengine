#	pragma once

#	include "Node.h"

namespace Menge
{
	class Scriptable
		: public virtual Node
	{
	public:
		Scriptable();
		~Scriptable();

	public:
		void setScriptable( ScriptObject * _scriptable ) override;
		ScriptObject * getScriptable() override;
		bool isScriptable() const override;

	protected:
		ScriptObject * m_scriptable;
	};
}