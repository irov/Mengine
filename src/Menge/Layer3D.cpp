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
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer3D::render()
	{}
	//////////////////////////////////////////////////////////////////////////
	bool Layer3D::handleKeyEvent( size_t _key, size_t _char,  bool _isDown )
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
	void Layer3D::loader( XmlElement *_xml)
	{
		SceneNode3D::loader(_xml);
		Layer::loader(_xml);
	}
	//////////////////////////////////////////////////////////////////////////
}