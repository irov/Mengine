#	include "Chapter.h"

#	include "ObjectImplement.h"

#	include "SceneManager.h"

#	include "Scene.h"

#	include "FileEngine.h"

#	include <assert.h>

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Chapter)
//////////////////////////////////////////////////////////////////////////()
Chapter::Chapter()
{
}
//////////////////////////////////////////////////////////////////////////
bool Chapter::addChildren(Node *_node)
{
	if( dynamic_cast<Scene*>(_node) == 0 )
	{
		assert(!"Can't add non-chapter node");
		return false;
	}

	return NodeImpl::addChildren(_node);
};