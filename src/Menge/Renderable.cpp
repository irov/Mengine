#	include "Renderable.h"
#	include "ObjectImplement.h"

#	include "SceneManager.h"

OBJECT_IMPLEMENT(Renderable);

void Renderable::render()
{
	//Empty
}

void Renderable::setSceneManager(SceneManager *_sceneManager)
{
	Allocator::setSceneManager(_sceneManager);

	m_renderEngine = _sceneManager->getRenderEngine();
}
