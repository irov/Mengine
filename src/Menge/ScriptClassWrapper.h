#	pragma once

#	include <string>

namespace Menge
{
	class ScriptObject;
	class Node;

	class ScriptClassWrapper
	{
	public:
		virtual ScriptObject * wrapp( Node * _node ) = 0;
	};

	class ScriptClassWrapperFactory
	{
	public:
		static void regWrapping( const std::string & _type, ScriptClassWrapper * _wrapper );
		static void wrapp( const std::string & _type, Node * _node );
	};
}