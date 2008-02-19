#	include "Layer3D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "Scene.h"

#	include "Camera2D.h"

#	include "Player.h"

#	include "RenderEngine.h"

#	include "LogEngine.h"
#	include "Game.h"

#	include "NodeForeach.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Layer3D);
	//////////////////////////////////////////////////////////////////////////
	Layer3D::Layer3D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::update( float _timing )
	{
		NodeCore::update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer3D::handleKeyEvent( size_t _key, size_t _char, bool _isDown )
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
	void Layer3D::loader( XmlElement * _xml )
	{
		NodeCore::loader(_xml);
		Layer::loader(_xml);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer3D::_activate()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::_addChildren( Node * _node )
	{
//		SceneNodeInterface * root
//			= Holder<RenderEngine>::hostage()->getRootSceneNode();
//будет ли тут в рут добавлятся?
		SceneNode3D * node = dynamic_cast<SceneNode3D*>(_node);
		this->addChild( node );
		//m_interface->addChild( node->m_interface );
	}
}