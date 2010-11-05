#	include "Scene.h"

#	include "ScriptEngine.h"
#	include "BinParser.h"
#	include "PhysicEngine2D.h"
#	include "RenderEngine.h"

#	include "Layer2D.h"
#	include "Player.h"
#	include "Camera2D.h"
#	include "Application.h"

#	include "Consts.h"

#	include "Logger/Logger.h"
#	include "ResourceManager.h"
#	include "ResourceImageDefault.h"
#	include "Game.h"

#	include "SceneManager.h"
#	include "ScheduleManager.h"

#	include "NodeManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scene::Scene()
		: MousePickerAdapter(false)
		, m_mainLayer(0)
		, m_parentScene(0)
		, m_offsetPosition(0.f,0.f)
		, m_gravity2D(0.f, 0.f)
		, m_physWorldBox2D(0.f, 0.f, 0.f, 0.f)
		, m_physWorld2D(false)
		, m_rtName(Consts::get()->c_Window)
		, m_rtSize(0.f, 0.f)
		, m_eventOnUpdate(false)
		, m_blockInput(false)
		, m_camera2D(NULL)
		, m_scheduleManager(NULL)
		, m_physicCanSleep(true)
	{
		const Resolution& res = Game::get()
			->getContentResolution();

		m_camera2D = NodeManager::get()
			->createNodeT<Camera2D>( "SceneCamera", Consts::get()->c_Camera2D );

		m_camera2D->setViewportSize( mt::vec2f(res[0], res[1]) );

		Player::get()->getRenderCamera2D()
			->addChildren( m_camera2D );

		m_scheduleManager = new ScheduleManager();
	}
	//////////////////////////////////////////////////////////////////////////
	Scene::~Scene()
	{
		if( m_scheduleManager != NULL )
		{
			delete m_scheduleManager;
			m_scheduleManager = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setMainLayer( Layer * _layer )
	{
		m_mainLayer = _layer;	
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_changeParent( Node * _oldParent, Node * _newParent )
	{
		m_parentScene = _newParent->getScene();

		if( m_parentScene == 0 )
		{
			return;
		}

		callMethod( ("onSubScene"), "(O)", m_parentScene->getEmbed() );
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
	bool Scene::pick( Arrow * _arrow )
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::_pickerActive() const
	{
		if( this->getBlockInput() == true )
		{
			return false;
		}

		if( this->isUpdatable() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_destroy()
	{
		Node::_destroy();

		if( m_camera2D != NULL )
		{
			//Player::get()->getRenderCamera2D()
			//	->removeChildren( m_camera2D );
			m_camera2D->destroy();
			m_camera2D = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::_activate()
	{
		const ConstString& name = this->getName();

		if( name.empty() )
		{
			return false;
		}

		MousePickerAdapter::activatePicker();

		// scene must be already active on onActivate event

		m_camera2D->activate();

		m_active = Node::_activate();

		callEvent( EVENT_ACTIVATE, "()" );

		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_deactivate()
	{
		m_scheduleManager->removeAll();

		MousePickerAdapter::deactivatePicker();

		callEvent( EVENT_DEACTIVATE, "()" );

		m_camera2D->deactivate();

		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::compile()
	{
		if( this->isCompile() == true )
		{
			return true;
		}

		if( PhysicEngine2D::get()->isWorldCreate() == false )
		{
			if( createPhysicsWorld() == false )
			{
				return false;
			}
		}

		bool result = false;

		result = Node::compile();

		if( result == true )
		{
			this->callEvent( EVENT_COMPILE, "()" );
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::_compile()
	{
		if( m_mainLayer == NULL )
		{
			MENGE_LOG_ERROR( "Main Layer is NULL in scene '%s'"
				, getName().c_str() 
				);

			return false;
		}

		mt::vec2f mainSize = m_mainLayer->getSize();

		Camera2D * camera2D = Player::get()
			->getRenderCamera2D();

		const Viewport & viewport = camera2D->getViewport();

		mt::vec2f viewport_size = viewport.end - viewport.begin;
		//assert( viewport_size.x >= 1024.0f );

		mainSize -= viewport_size;

		//if( fabsf( mainSize.x /* mainSize.y*/) > 0.0001f )
		{
			for( TListChild::reverse_iterator 
				it = m_child.rbegin(),
				it_end = m_child.rend();
			it != it_end;
			++it)
			{
				if( Layer2D * layer2D = dynamic_cast<Layer2D*>( *it ) )
				{
					mt::vec2f layerSize = layer2D->getSize();

					layerSize -= viewport_size;

					float factorX = ( mainSize.x > 0.001f ) ? ( layerSize.x / mainSize.x ) : 0.0f;
					float factorY = ( mainSize.y > 0.001f ) ? ( layerSize.y / mainSize.y ) : 0.0f;

					mt::vec2f parallaxFactor( factorX, factorY );

					layer2D->setParallaxFactor( parallaxFactor );
				}
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_release()
	{
		if( m_physWorld2D )
		{
			PhysicEngine2D::get()
				->destroyWorld();
		}

		Node::_release();

		this->callEvent( EVENT_RELEASE, "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_update( float _timing )
	{
		// update physics first
		if( m_physWorld2D )
		{
			PhysicEngine2D::get()
				->update( _timing );
		}

		Node::_update( _timing );
		//m_camera2D->update( _timing );
		if( m_eventOnUpdate )
		{
			callEvent( EVENT_UPDATE, "(f)", _timing );
		}

		m_scheduleManager->update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_postUpdate( float _timing )
	{
		MousePickerAdapter::updatePicker();
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::loader( BinParser * _parser )
	{
		Node::loader(_parser);

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Gravity2D_Value, m_gravity2D );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::PhysicWorld2DBox_Value, &Scene::setPhysicsWorld );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::PhysicCanSleep_Value, &Scene::setPhysicsCanSleep );

			BIN_CASE_ATTRIBUTE( Protocol::RenderTarget_Name, m_rtName );
			BIN_CASE_ATTRIBUTE( Protocol::RenderTarget_Size, m_rtSize );

			BIN_CASE_ATTRIBUTE_METHOD_CHECK( Protocol::SubScene_Name, &Scene::addSubScene_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setPhysicsWorld( const mt::vec4f & _box )
	{
		m_physWorldBox2D = _box;
		m_physWorld2D = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::createPhysicsWorld()
	{
		if( m_physWorld2D == false )
		{
			return true;
		}

		mt::vec2f minBox( m_physWorldBox2D.x, m_physWorldBox2D.y );
		mt::vec2f maxBox( m_physWorldBox2D.z, m_physWorldBox2D.w );

		PhysicEngine2D::get()
			->createWorld( minBox, maxBox, m_gravity2D, m_physicCanSleep );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_addChildren( Node * _node )
	{
		if( Layer * layer = dynamic_cast<Layer*>(_node) )
		{
			layer->setScene( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_embedding( PyObject * _embed )
	{
		Entity::_embedding( _embed );

		Eventable::registerEvent( EVENT_KEY, ("onHandleKeyEvent"), _embed );
		Eventable::registerEvent( EVENT_MOUSE_BUTTON, ("onHandleMouseButtonEvent"), _embed );
		Eventable::registerEvent( EVENT_MOUSE_BUTTON_END, ("onHandleMouseButtonEventEnd"), _embed );
		Eventable::registerEvent( EVENT_MOUSE_MOVE, ("onHandleMouseMove"), _embed );

		m_eventOnUpdate = Eventable::registerEvent( EVENT_UPDATE, ("onUpdate"), _embed );

		Eventable::registerEvent( EVENT_MOUSE_LEAVE, ("onMouseLeave"), _embed );
		Eventable::registerEvent( EVENT_MOUSE_ENTER, ("onMouseEnter"), _embed );
		Eventable::registerEvent( EVENT_FOCUS, ("onFocus"), _embed );

		Eventable::registerEvent( EVENT_ACTIVATE, ("onActivate"), _embed );
		Eventable::registerEvent( EVENT_DEACTIVATE, ("onDeactivate"), _embed );
		Eventable::registerEvent( EVENT_COMPILE, ("onCompile"), _embed );
		Eventable::registerEvent( EVENT_RELEASE, ("onRelease"), _embed );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setRenderTarget( const ConstString& _cameraName, const mt::vec2f& _size )
	{
		m_rtName = _cameraName;
		m_rtSize = _size;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::render( Camera2D * _camera )
	{
		if( isRenderable() == false )
		{
			return;
		}

		//const mt::vec2f& main_size = m_mainLayer->getSize();

		Camera2D * camera2D = Player::get()
			->getRenderCamera2D();

		//mt::vec2f camPos = camera2D->getLocalPosition();
		//const Viewport & vp = camera2D->getViewport();

		//mt::vec2f vp_size = vp.end - vp.begin;

		//if( ( camPos.y - vp_size.y * 0.5f ) < 0.0f )
		//{
		//	camera2D->setLocalPosition( mt::vec2f( camPos.x, vp_size.y * 0.5f ) );
		//}
		//else if( ( camPos.y + vp_size.y * 0.5f ) > main_size.y )
		//{
		//	camera2D->setLocalPosition( mt::vec2f( camPos.x, main_size.y - vp_size.y * 0.5f ) );
		//	//viewport.begin.y = main_size.y - viewport_size.y;
		//}

		_render( camera2D );

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
				->setRenderTarget( m_rtName );

			(*it)->render( m_camera2D );
		}

		//const mt::vec2f & pos = camera2D->getLocalPosition();

		//if( cmp_v2_v2(pos, camPos) == false )
		//{
		//	camera2D->setLocalPosition( camPos );
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::renderSelf()
	{
		render( m_camera2D );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onMouseLeave()
	{
		bool handle = false;

		if( askEvent( handle, EVENT_MOUSE_LEAVE, "()" ) == false )
		{
			handle = false;
		}

		if( handle == false )
		{
			for( TListChild::iterator 
				it = m_child.begin(), 
				it_end = m_child.end();
			it != it_end;
			it++ )
			{
				Scene * subScene = dynamic_cast<Scene*>( *it );
				if( subScene != 0 )
				{
					subScene->onMouseLeave();
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onMouseEnter()
	{
		bool handle = false;

		if( askEvent( handle, EVENT_MOUSE_ENTER, "()" ) )
		{
			handle = false;
		}

		if( handle == false )
		{
			for( TListChild::iterator it = m_child.begin(), it_end = m_child.end();
				it != it_end;
				it++ )
			{
				Scene* subScene = dynamic_cast<Scene*>( *it );
				if( subScene != 0 )
				{
					subScene->onMouseEnter();
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& Scene::getRenderTarget() const
	{
		return m_rtName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::blockInput( bool _block )
	{
		m_blockInput = _block;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::getBlockInput() const
	{
		return m_blockInput;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onFocus( bool _focus )
	{
		bool handle = false;

		if( askEvent( handle, EVENT_FOCUS, "(b)", _focus ) == false )
		{
			handle = false;
		}

		if( handle == false )
		{
			for( TListChild::iterator it = m_child.begin(), it_end = m_child.end();
				it != it_end;
				it++ )
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
	//////////////////////////////////////////////////////////////////////////
	Camera2D* Scene::getCamera()
	{
		return m_camera2D;
	}
	//////////////////////////////////////////////////////////////////////////
	ScheduleManager * Scene::getScheduleManager()
	{
		return m_scheduleManager;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setCameraPosition( float _x, float _y )
	{
		m_camera2D->setLocalPosition( mt::vec2f( _x, _y ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::enableCameraFollowing( bool _enable, float _force )
	{
		m_camera2D->enableTargetFollowing( _enable, _force );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setCameraTarget( Node* _target )
	{
		m_camera2D->setTarget( _target );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setCameraBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower )
	{
		m_camera2D->setBounds( _leftUpper, _rightLower );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onLeave()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::onEnter()
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			if( this->askEvent( handle, EVENT_KEY, "(IIb)", _key, _char, _isDown ) == false )
			{
				handle = m_defaultHandle;
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			if( this->askEvent( handle, EVENT_MOUSE_BUTTON, "(Ib)", _button, _isDown ) == false )
			{
				handle = m_defaultHandle;
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseButtonEventEnd( unsigned int _button, bool _isDown )
	{
		this->callEvent( EVENT_MOUSE_BUTTON_END, "(Ib)", _button, _isDown );

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseMove( float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			if( this->askEvent( handle, EVENT_MOUSE_MOVE, "(ffi)", _x, _y, _whell ) == false )
			{
				handle = m_defaultHandle;
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setPhysicsCanSleep( bool _canSleep )
	{
		m_physicCanSleep = _canSleep;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::addSubScene_( const ConstString & _sceneName )
	{
		Scene * scene = SceneManager::get()
			->getScene( _sceneName );
		
		if( scene == 0 )
		{
			MENGE_LOG_ERROR( "Invalid add sub scene '%s'"
				, _sceneName.c_str()
				);

			return false;
		}

		this->addChildren( scene );

		return true;
	}
}
