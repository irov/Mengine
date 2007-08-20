#	include "NodeChildren.h"

#	include "ScriptEngine.h"

#	include "SceneManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Node * NodeChildrenImpl::createChildrenImpl( const std::string & _type )
	{
		Node * node = SceneManager::createNode( _type );
		return node;
	}
}