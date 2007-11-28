#	include "Scene2D.h"

#	include "ObjectImplement.h"


namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Scene2D);
	//////////////////////////////////////////////////////////////////////////

	void Scene2D::loader( TiXmlElement *_xml )
	{
		Scene::loader(_xml);
		Allocator2D::loader(_xml);
	}

	bool Scene2D::handleKeyEvent( size_t _key, bool _isDown )
	{
		return Scene::handleKeyEvent( _key, _isDown );
	}

	bool Scene2D::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		return Scene::handleMouseButtonEvent( _button, _isDown );
	}

	bool Scene2D::handleMouseMove( int _x, int _y, int _whell )
	{
		return Scene::handleMouseMove( _x, _y, _whell );
	}

	bool Scene2D::_activate()
	{
		return Scene::_activate();
	}

	void Scene2D::_update( float _timing )
	{
		Scene::_update(_timing);
		//Allocator2D::_update(_timing); 
	}

	void Scene2D::_addChildren( Layer * _layer )
	{
		Scene::_addChildren( _layer );
	}


} // namespace Menge