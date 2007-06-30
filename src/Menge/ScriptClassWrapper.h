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

	class ScriptClassWrapper
	{
	public:
		virtual PyObject * wrapp( Node * _node ) = 0;
	};

	class ScriptClassWrapperFactory
	{
	public:
		static void regWrapping( const std::string & _type, ScriptClassWrapper * _wrapper );
		static void wrapp( const std::string & _type, Node * _node );
	};
}