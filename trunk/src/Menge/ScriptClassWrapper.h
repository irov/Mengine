#	pragma once

#	include "Config/Typedef.h"
//#	include <string>

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
		virtual PyObject * wrap( Node * _node ) = 0;
	};

	class ScriptClassWrapperFactory
	{
	public:
		static void regWrapping( const String& _type, ScriptClassWrapper * _wrapper );
		static PyObject * wrap( const String& _type, Node * _node );
	};
}
