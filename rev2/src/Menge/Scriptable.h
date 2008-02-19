#	pragma once

#	include <string>

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class Node;

	class Scriptable
	{
	public:
		Scriptable();
		virtual ~Scriptable();//virtual??? зачем?

		void setWrapp( Node * _node );

	public:
		void setScript( PyObject * _scriptable );
		PyObject * getScript();
		bool isScriptable() const;

	public:
		void callMethod( const std::string & _method, const char * _format, ... );

	protected:
		PyObject * m_scriptable;

	private:
		Node * m_node;
	};
}