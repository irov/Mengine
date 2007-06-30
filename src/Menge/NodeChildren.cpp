#	include "NodeChildren.h"

#	include "Holder.h"
#	include "ScriptEngine.h"

#	include "SceneManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	bool NodeChildrenImpl::increfImpl( Node * _node )
	{
		Holder<ScriptEngine>::hostage()->
			incref( _node );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * NodeChildrenImpl::createChildrenImpl( const std::string & _type )
	{
		Node * node = SceneManager::createNode( _type );

		return node;
	}
	
}