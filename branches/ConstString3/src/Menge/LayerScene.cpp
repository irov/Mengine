#	include "LayerScene.h"

#	include "BinParser.h"

#	include "Logger/Logger.h"
#	include "ScriptEngine.h"

#	include "Scene.h"
#	include "SceneManager.h"

#	include "Core/Holder.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LayerScene::LayerScene()
		: m_subScene(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::loader( BinParser * _parser )
	{
		Layer::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Scene_Name, &LayerScene::loadScene_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::loadScene_( const ConstString & _scene )
	{
		m_sceneName = _scene;

		if( m_scene == 0 )
		{
			MENGE_LOG_ERROR( "Error: appending LayerScene '%s' node to not Scene node '%s'"
				, _scene.c_str() 
				, getName().c_str()
				);

			return;
		}

		m_subScene = SceneManager::get()
			->getScene( _scene );

		if( m_subScene )
		{
			m_subScene->setParentScene( m_scene );
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
	void LayerScene::render( Camera2D * _camera )
	{	
		if( m_subScene )
		{
			m_subScene->render( _camera );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool LayerScene::_activate()
	{
		if( m_subScene )
		{
			return m_subScene->activate();
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
		if( m_subScene )
		{
			return m_subScene->compile();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::_release()
	{
		if( m_subScene )
		{
			m_subScene->release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::_destroy()
	{
		if( m_subScene )
		{
			const ConstString & sceneName = m_subScene->getName();

			SceneManager::get()
				->destroyScene( sceneName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Node * LayerScene::getChildren( const ConstString& _name, bool _recursive ) const
	{
		if( m_subScene )
		{
			return m_subScene->getChildren( _name, _recursive );
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LayerScene::isChildren( Node * _node, bool _recursive ) const
	{
		if( m_subScene )
		{
			return m_subScene->isChildren( _node, _recursive );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::onMouseLeave()
	{
		if( m_subScene )
		{
			m_subScene->onMouseLeave();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::onMouseEnter()
	{
		if( m_subScene )
		{
			m_subScene->onMouseEnter();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::onFocus( bool _focus )
	{
		if( m_subScene )
		{
			m_subScene->onFocus( _focus );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void LayerScene::visitChildren( Visitor * _visitor )
	{
		if( m_subScene )
		{
			m_subScene->visitChildren( _visitor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
