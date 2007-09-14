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
		virtual ~Scriptable();

	public:
		void setScriptable( PyObject * _scriptable );
		PyObject * getScriptable();
		bool isScriptable() const;

	protected:
		PyObject * m_scriptable;
	};
}