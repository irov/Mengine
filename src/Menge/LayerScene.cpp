#	include "LayerScene.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "ScriptEngine.h"

#	include "Scene.h"

#	include "Game.h"

#	include "Holder.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(LayerScene);
	//////////////////////////////////////////////////////////////////////////
	LayerScene::LayerScene()
	: m_subScene(0)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool LayerScene::handleKeyEvent( size_t _key, bool _isDown )
	{
		if( m_subScene )
		{
			return m_subScene->handleKeyEvent( _key, _isDown );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LayerScene::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		if( m_subScene )
		{
			return m_subScene->handleMouseButtonEvent( _button, _isDown );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LayerScene::handleMouseMove( int _x, int _y, int _whell )
	{
		if( m_subScene )
		{
			return m_subScene->handleMouseMove( _x, _y, _whell );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::loader( XmlElement * _xml )
	{
		Layer::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_VALUE_NODE( "Scene", "Name", m_sceneName );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::update( float _timing )
	{
		if( m_subScene )
		{
			m_subScene->update( _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::render()
	{	
		if( m_subScene )
		{
			m_subScene->render();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::_destroy()
	{
		if( m_subScene )
		{
			Holder<Game>::hostage()->destroyScene( m_subScene->getName() );
		}
		m_subScene = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LayerScene::_activate()
	{
		if( m_sceneName.empty() == false )
		{
			m_subScene = Holder<Game>::hostage()
				->getScene( m_sceneName );

			m_subScene->activate();

			m_subScene->setParentScene( m_scene );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::_deactivate()
	{
		if( m_subScene )
		{
			m_subScene->deactivate();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool LayerScene::_compile()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::_release()
	{
		if( m_subScene )
		{
			return m_subScene->release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Node * LayerScene::getChildren( const std::string & _name, bool _recursion )
	{
		if( m_subScene )
		{
			return m_subScene->getChildren( _name, _recursion );
		}

		return 0;
	}
}