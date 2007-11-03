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
		void setScript( PyObject * _scriptable );
		PyObject * getScript();
		bool isScriptable() const;

	public:
		void callMethod( const std::string & _method, const char * _format, ... );

	protected:
		PyObject * m_scriptable;
	};
}