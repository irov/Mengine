#	include "Scene.h"

#	include "ScriptEngine.h"

#	include "PhysicEngine2D.h"
#	include "RenderEngine.h"

#	include "Layer2D.h"
#	include "Player.h"
#	include "Camera2D.h"
#	include "Application.h"

#	include "Consts.h"

#	include "LogEngine.h"
#	include "ResourceManager.h"
#	include "ResourceImageDefault.h"
#	include "Game.h"

#	include "SceneManager.h"

#	include "NodeManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scene::Scene()
		: m_mainLayer(0)
		, m_parentScene(0)
		, m_offsetPosition(0.f,0.f)
	{
		//const Resolution& res = Game::get()
		//	->getContentResolution();

		//ConstString c_SceneCamera("SceneCamera");
		//m_camera2D = NodeManager::get()
		//	->createNodeT<Camera2D>( c_SceneCamera, Consts::get()->c_Camera2D, Consts::get()->c_builtin_empty );

		//m_camera2D->setViewportSize( mt::vec2f(res[0], res[1]) );

		//Player::get()->getRenderCamera2D()
		//	->addChildren( m_camera2D );
	}
	//////////////////////////////////////////////////////////////////////////
	Scene::~Scene()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setMainLayer( Layer * _layer )
	{
		m_mainLayer = _layer;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_changeParent( Node * _oldParent, Node * _newParent )
	{
		if( _newParent == 0 )
		{
			return;
		}

		m_parentScene = _newParent->getScene();

		if( m_parentScene == 0 )
		{
			return;
		}

		EVENTABLE_CALL(this, EVENT_ON_SUB_SCENE)( "(O)", m_parentScene->getEmbed() );
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Scene::getParentScene() const
	{
		return m_parentScene;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::isSubScene() const
	{
		return m_parentScene != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Scene::getScene()
	{
		return this;
	}
	//////////////////////////////////////////////////////////////////////////
	Layer * Scene::getMainLayer()
	{
		return m_mainLayer;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_addChildren( Node * _node )
	{
		if( Layer * layer = dynamic_cast<Layer*>(_node) )
		{
			layer->setScene( this );

			if( layer->isMain() == true )
			{
				this->setMainLayer( layer ); 
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_setEventListener( PyObject * _embed )
	{
		Entity::_setEventListener( _embed );

		Eventable::registerEvent( EVENT_APP_MOUSE_LEAVE, ("onAppMouseLeave"), _embed );
		Eventable::registerEvent( EVENT_APP_MOUSE_ENTER, ("onAppMouseEnter"), _embed );
		Eventable::registerEvent( EVENT_FOCUS, ("onFocus"), _embed );

		Eventable::registerEvent( EVENT_ON_SUB_SCENE, ("onSubScene"), _embed );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onAppMouseLeave()
	{
		bool handle = false;

		//if( askEvent( handle, EVENT_APP_MOUSE_LEAVE, "()" ) == false )
		//{
		//	handle = false;
		//}

		EVENTABLE_ASK(this, EVENT_APP_MOUSE_LEAVE)( handle, false, "()" );

		if( handle == false )
		{
			for( TSlugChild it(m_child); it.end() == false; it.next_shuffle() )
			{
				Scene * subScene = dynamic_cast<Scene*>( *it );

				if( subScene != 0 )
				{
					subScene->onAppMouseLeave();
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onAppMouseEnter()
	{
		bool handle = false;

		//if( askEvent( handle, EVENT_APP_MOUSE_ENTER, "()" ) )
		//{
		//	handle = false;
		//}

		EVENTABLE_ASK(this, EVENT_APP_MOUSE_ENTER)( handle, false, "()" );

		if( handle == false )
		{
			for( TSlugChild it(m_child); it.end() == false; it.next_shuffle() )
			{
				Scene* subScene = dynamic_cast<Scene*>( *it );

				if( subScene != 0 )
				{
					subScene->onAppMouseEnter();
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onFocus( bool _focus )
	{
		bool handle = false;

		//if( askEvent( handle, EVENT_FOCUS, "(O)", pybind::get_bool(_focus) ) == false )
		//{
		//	handle = false;
		//}

		EVENTABLE_ASK(this, EVENT_FOCUS)( handle, false, "(O)", pybind::get_bool(_focus) );

		if( handle == false )
		{
			for( TSlugChild it(m_child); it.end() == false; it.next_shuffle() )
			{
				Scene* subScene = dynamic_cast<Scene*>( *it );

				if( subScene != 0 )
				{
					subScene->onFocus( _focus );
				}
			}			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_render( RenderCameraInterface * _camera )
	{
		// nothing
	}
}
