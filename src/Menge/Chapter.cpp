#	include "Chapter.h"
#	include "ObjectImplement.h"

#	include "SceneManager.h"

#	include "Scene.h"

#	include "FileEngine.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Chapter)
//////////////////////////////////////////////////////////////////////////()
Chapter::Chapter()
{
	m_childrenForeach = false;
}
//////////////////////////////////////////////////////////////////////////
bool Chapter::addChildren(Node *_node)
{
	if( dynamic_cast<Scene*>(_node) == 0 )
	{
		return false;
	}

	return NodeImpl::addChildren(_node);
};