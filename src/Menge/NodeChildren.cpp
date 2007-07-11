#	include "NodeChildren.h"

#	include "ScriptEngine.h"

#	include "SceneManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void NodeChildrenImpl::increfImpl( Node * _node )
	{
		Holder<ScriptEngine>::hostage()->
			incref( _node );
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeChildrenImpl::decrefImpl( Node * _node )
	{
		Holder<ScriptEngine>::hostage()->
			decref( _node );
	}
	//////////////////////////////////////////////////////////////////////////
	Node * NodeChildrenImpl::createChildrenImpl( const std::string & _type )
	{
		Node * node = SceneManager::createNode( _type );

		return node;
	}
	
}