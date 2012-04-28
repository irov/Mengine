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
		//, m_gravity2D(0.f, 0.f)
		//, m_physWorldBox2D(0.f, 0.f, 0.f, 0.f)
		//, m_physWorld2D(false)
		, m_renderTargetName(Consts::get()->c_Window)
		, m_renderTargetSize(0.f, 0.f)
		, m_camera2D(NULL)
		//, m_physicCanSleep(true)
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

		this->callEvent( EVENT_ON_SUB_SCENE, "(O)", m_parentScene->getEmbed() );
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
	void Scene::setRenderTarget( const ConstString& _cameraName, const mt::vec2f& _size )
	{
		m_renderTargetName = _cameraName;
		m_renderTargetSize = _size;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::render( Camera2D * _camera )
	{
		if( isRenderable() == false )
		{
			return;
		}

		Camera2D * renderCamera = _camera;

		Viewport old_rp;
		mt::mat4f old_vm;
		mt::mat4f old_pm;

		if( m_camera2D )
		{
			renderCamera = m_camera2D;

			//renderCamera = m_camera2D;
			RenderEngine::get()
				->getCurrentRenderPass( old_rp, old_vm, old_pm );

			const Viewport & camera_vp = m_camera2D->getViewport();
			const mt::mat4f & camera_vm= m_camera2D->getViewMatrix();
			const mt::mat4f & camera_pm= m_camera2D->getProjectionMatrix();

			//Viewport new_vp;
			//mt::mul_v2_m3( new_vp.begin, camera_viewport.begin, wm );
			//mt::mul_v2_m3( new_vp.end, camera_viewport.end, wm );

			RenderEngine::get()
				->newRenderPass( camera_vp, camera_vm, camera_pm );
		}

		this->_render( renderCamera );

		for( TListChild::iterator
			it = m_child.begin(),
			it_end = m_child.end();
		it != it_end;
		++it)
		{
			if( (*it)->isRenderable() == false ) 
			{
				continue;
			}

			RenderEngine::get()
				->setRenderTarget( m_renderTargetName );

			(*it)->render( renderCamera );
		}

		if( m_camera2D )
		{
			RenderEngine::get()
				->newRenderPass( old_rp, old_vm, old_pm );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::renderSelf( Camera2D * _camera )
	{
		render( _camera );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onAppMouseLeave()
	{
		bool handle = false;

		if( askEvent( handle, EVENT_APP_MOUSE_LEAVE, "()" ) == false )
		{
			handle = false;
		}

		if( handle == false )
		{
			for( TListChild::iterator 
				it = m_child.begin(), 
				it_end = m_child.end();
			it != it_end;
			++it )
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

		if( askEvent( handle, EVENT_APP_MOUSE_ENTER, "()" ) )
		{
			handle = false;
		}

		if( handle == false )
		{
			for( TListChild::iterator 
				it = m_child.begin(), 
				it_end = m_child.end();
			it != it_end;
			++it )
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
	void Scene::setCamera2D( Camera2D * _camera2D )
	{
		m_camera2D = _camera2D;

		this->addChildren( _camera2D );
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& Scene::getRenderTarget() const
	{
		return m_renderTargetName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onFocus( bool _focus )
	{
		bool handle = false;

		if( askEvent( handle, EVENT_FOCUS, "(O)", pybind::get_bool(_focus) ) == false )
		{
			handle = false;
		}

		if( handle == false )
		{
			for( TListChild::iterator 
				it = m_child.begin(), 
				it_end = m_child.end();
			it != it_end;
			++it )
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
	void Scene::_render( Camera2D * _camera )
	{
		// nothing
	}
}
