#	include "Chapter.h"

#	include "ObjectImplement.h"

#	include "Scene.h"

#	include <assert.h>

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Chapter)
//////////////////////////////////////////////////////////////////////////()
Chapter::Chapter()
{
}
//////////////////////////////////////////////////////////////////////////
Scene * Chapter::getScene( const std::string & _name )
{
	Scene * scene = getChildrenT<Scene>(_name);
	return scene;
}
//////////////////////////////////////////////////////////////////////////
bool Chapter::addChildren(Node *_node)
{
	if( dynamic_cast<Scene*>(_node) == 0 )
	{
		assert(!"Can't add non-chapter node, please add `Scene` based node");
		return false;
	}

	return NodeImpl::addChildren(_node);
};