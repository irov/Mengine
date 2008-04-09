#	pragma once

#	include "Node.h"

#	include <string>

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
		void setScript( PyObject * _scriptable );
		PyObject * getScript();
		bool isScriptable() const;

	public:
		void callMethod( const std::string & _method, const char * _format, ... );

	protected:
		PyObject * m_scriptable;
	};
}