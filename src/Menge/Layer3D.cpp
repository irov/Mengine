#	include "Layer3D.h"
#	include "ObjectImplement.h"

#	include "Camera3D.h"

#	include "Player.h"

#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Layer3D);
	//////////////////////////////////////////////////////////////////////////
	Layer3D::Layer3D()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::renderLayer()
	{
		Camera3D * camera = 
			Holder<Player>::hostage()
			->getRenderCamera3D();

		Holder<RenderEngine>::hostage()->setViewMatrix(camera->getViewMatrix());
		Holder<RenderEngine>::hostage()->setProjectionMatrix(camera->getProjectionMatrix());
		
		for each( SceneNode3D * node in m_listChildren )
		{
			const mt::mat4f & wm = node->getWorldMatrix();
			node->render( wm, camera );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer3D::handleKeyEvent( size_t _key, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer3D::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer3D::handleMouseMove( int _x, int _y, int _whell )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::loader( TiXmlElement *_xml)
	{
		SceneNode3D::loader(_xml);
		Layer::loader(_xml);
	}
}