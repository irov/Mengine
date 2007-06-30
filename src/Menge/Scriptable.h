#	pragma once

#	include "Node.h"

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class Scriptable
		: public virtual Node
	{
	public:
		Scriptable();
		~Scriptable();

	public:
		void setScriptable( PyObject * _scriptable ) override;
		PyObject * getScriptable() override;
		bool isScriptable() const override;

	protected:
		PyObject * m_scriptable;
	};
}