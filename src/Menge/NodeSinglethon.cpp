#	include "NodeSinglethon.h"

#	include "NodeForeach.h"

#	include "Holder.h"

#	include "ScriptEngine.h"

#	include "SceneManager.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void NodeSinglethon::setParent( Node * _parent )
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeSinglethon::addChildren( Node * _node )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeSinglethon::removeChildren( Node *_node)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeSinglethon::isChildren( Node *_node)
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeSinglethon::foreachChildren( const NodeForeach & _foreach )
	{
	}
}