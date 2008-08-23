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
	bool LayerScene::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		if( m_subScene )
		{
			return m_subScene->handleKeyEvent( _key, _char, _isDown );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LayerScene::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		if( m_subScene )
		{
			return m_subScene->handleMouseButtonEvent( _button, _isDown );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LayerScene::handleMouseMove( float _x, float _y, int _whell )
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
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("Scene"), MENGE_TEXT("Name"), m_sceneName );
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
	void LayerScene::render( unsigned int _debugMask )
	{	
		if( m_subScene )
		{
			m_subScene->render( _debugMask );
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

			m_subScene->setParentScene( m_scene );

			m_subScene->activate();

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
	Node * LayerScene::getChildren( const String& _name, bool _recursion ) const
	{
		if( m_subScene )
		{
			return m_subScene->getChildren( _name, _recursion );
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LayerScene::handleMouseButtonEventEnd( unsigned int _button, bool _isDown )
	{
		if( m_subScene )
		{
			return m_subScene->handleMouseButtonEventEnd( _button, _isDown );
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
}