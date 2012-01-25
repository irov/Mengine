#	include "Player.h"

#	include "NodeManager.h"

#	include "Game.h"
#	include "Application.h"

#	include "Scene.h"
#	include "Join.h"

#	include "Camera2D.h"
#	include "Consts.h"

#	include "Arrow.h"

#	include "LogEngine.h"

#	include "TextField.h"

#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"

#	include "ResourceManager.h"
#	include "ScriptEngine.h"
#	include "ParticleEngine.h"

#	include "MousePickerSystem.h"
#	include "GlobalHandleSystem.h"

#	include "ScheduleManager.h"
#	include "TimingManager.h"

#	include "SceneManager.h"
#	include "EventManager.h"

#	include "Math/mat3.h"

#	include <sstream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player()
		: m_scene(0)
		, m_arrow(0)
		, m_renderCamera2D(0)
		, m_switchScene(false)
		, m_removeScene(false)
		, m_destroyOldScene(false)
		, m_restartScene(false)
		, m_arrowHided(false)
		, m_changeSceneCb(NULL)
		, m_removeSceneCb(NULL)
		, m_fps(0)
#	ifndef MENGE_MASTER_RELEASE
		, m_showDebugText(false)
		, m_debugText(NULL)
#	endif
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Player::~Player()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::setCurrentScene( const ConstString& _name, bool _destroyOld, bool _destroyAfterSwitch, PyObject* _cb )
	{
		if( m_switchScene == true )
		{
			return false;
		}

		m_switchSceneName = _name;
		
		if( m_scene != NULL && m_switchSceneName == m_scene->getName() )
		{
			m_restartScene = true;
		}

		m_switchScene = true;

		m_destroyOldScene = _destroyOld;
		m_destroyAfterSwitch = _destroyAfterSwitch;

		if( _cb != NULL && pybind::is_none(_cb) == false )
		{
			m_changeSceneCb = _cb;

			pybind::incref( m_changeSceneCb );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::removeCurrentScene( PyObject * _cb )
	{
		if( m_removeScene == true )
		{
			return false;
		}

		m_removeScene = true;

		if( _cb != NULL && pybind::is_none(_cb) == false )
		{
			m_removeSceneCb = _cb;

			pybind::incref( m_removeSceneCb );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Player::getCurrentScene()
	{
		return m_scene;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::updateChangeScene_()
	{
		if( m_switchScene == true )
		{
			this->updateSwitchScene_();
		}
		else if ( m_removeScene == true )
		{
			this->updateRemoveScene_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::updateRemoveScene_()
	{
		m_removeScene = false;

		if( m_scene != NULL )
		{
			if( m_arrow )
			{
				m_arrow->removeFromParent();
				m_arrow->disable();
			}				

			m_scheduleManager->removeAll();
			m_timingManager->removeAll(false);

			m_mousePickerSystem->clear();
			m_globalHandleSystem->clear();

			Game::get()
				->clearHomeless();

			m_scene->destroy();

			m_scene = NULL;
		}

		if( m_removeSceneCb != NULL )
		{
			PyObject * cb = m_removeSceneCb;
			m_removeSceneCb = NULL;

			pybind::call( cb, "()" );
			pybind::decref( cb );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::updateSwitchScene_()
	{
		m_switchScene = false;

		Scene * oldScene = m_scene;
		m_scene = 0;

		if( m_restartScene )		// just restart scene
		{		
			m_restartScene = false;

			m_switchSceneName = oldScene->getName();
		}

		if( m_arrow )
		{
			m_arrow->removeFromParent();
			m_arrow->disable();
		}

		m_scheduleManager->removeAll();
		m_timingManager->removeAll(false);

		if( oldScene && m_destroyOldScene == true && m_destroyAfterSwitch == false )
		{
			oldScene->destroy();
		}

		Game::get()
			->clearHomeless();

		m_mousePickerSystem->clear();
		m_globalHandleSystem->clear();

		if( oldScene && m_destroyOldScene == true && m_destroyAfterSwitch == true )
		{
			oldScene->destroy();
		}

		for( TVectorJoins::iterator
			it = m_joins.begin(),
			it_end = m_joins.end();
		it != it_end;
		++it )
		{
			delete (*it);
		}

		m_joins.clear();

		m_scene = SceneManager::get()
			->createScene( m_switchSceneName );

		PyObject * cb = m_changeSceneCb;
		m_changeSceneCb = NULL;

		if( m_scene == 0 )
		{
			MENGE_LOG_ERROR( "Player::updateChangeScene scene not found %s"
				, m_switchSceneName.c_str() 
				);

			if( cb != NULL )
			{
				pybind::call( cb, "(OO)", pybind::ret_none(), pybind::get_bool(false) );

				pybind::decref( cb );
			}

			return;
		}

		if( cb != NULL )
		{
			pybind::call( cb, "(OO)", m_scene->getEmbed(), pybind::get_bool(false) );
		}

		//Holder<ResourceManager>::get()->_dumpResources( "before compile next scene " + m_scene->getName() );

		m_scene->enable();

		if( m_arrow )
		{
			m_arrow->enable();
		}

		//Holder<ResourceManager>::get()->_dumpResources( "after compile next scene " + m_scene->getName() );

		if( cb != NULL )
		{
			pybind::call( cb, "(OO)", m_scene->getEmbed(), pybind::get_bool(true) );
		}
	
		if( cb != NULL )
		{
			pybind::decref( cb );
		}

		return;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::isChangedScene() const
	{
		return m_switchScene;	
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setArrow(Arrow * _arrow)
	{
		if( m_arrow )
		{
			m_arrow->disable();
		}

		m_arrow = _arrow;

		if( m_arrow )
		{
			if( m_scene )
			{
				m_arrow->enable();
			}

			m_arrow->setContentResolution( m_contentResolution );
			m_arrow->setCurrentResolution( m_currentResolution );
		}

		m_mousePickerSystem->setArrow( m_arrow );
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * Player::getArrow()
	{
		return m_arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	ScheduleManager * Player::getScheduleManager()
	{
		return m_scheduleManager;
	}
	//////////////////////////////////////////////////////////////////////////
	TimingManager * Player::getTimingManager()
	{
		return m_timingManager;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::initialize( Arrow * _arrow, const Resolution & _contentResolution, const Resolution & _currentResolution )
	{
		m_mousePickerSystem = new MousePickerSystem();
		MousePickerSystem::keep(m_mousePickerSystem);

		m_globalHandleSystem = new GlobalHandleSystem();
		GlobalHandleSystem::keep(m_globalHandleSystem);

		m_scheduleManager = new ScheduleManager();
		m_timingManager = new TimingManager();

		m_eventManager = new EventManager();
		EventManager::keep(m_eventManager);

		m_contentResolution = _contentResolution;
		m_currentResolution = _currentResolution;

		if( _arrow == 0 )
		{
			MENGE_LOG_ERROR( "Player::init default arrow not found" );
			return false;
		}

		float crx = float( m_contentResolution.getWidth() );
		float cry = float( m_contentResolution.getHeight() );

		ConstString c_playerCamera("playerCamera");

		Camera2D * camera = NodeManager::get()
			->createNodeT<Camera2D>( c_playerCamera, Consts::get()->c_Camera2D, Consts::get()->c_builtin_empty );

		camera->setRenderTarget( Consts::get()->c_Window );

		Viewport vp(0.f, 0.f, crx, cry);
		camera->setRenderport( vp );

		//mt::vec2f vp_pos(crx * 0.5f, cry * 0.5f);
		//camera->setLocalPosition(vp_pos);

		camera->enable();

		this->setRenderCamera2D( camera );
		this->setArrow( _arrow );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::finalize()
	{
		if( m_scene != NULL )
		{
			m_scene->destroy();
			m_scene = NULL;
		}

		if( m_removeSceneCb != NULL )
		{
			pybind::decref(m_removeSceneCb);
			m_removeSceneCb = NULL;
		}

		if( m_changeSceneCb != NULL )
		{
			pybind::decref(m_changeSceneCb);
			m_changeSceneCb = NULL;
		}

		if( m_renderCamera2D )
		{
			m_renderCamera2D->destroy();
			m_renderCamera2D = NULL;
		}

		if( m_mousePickerSystem )
		{
			delete m_mousePickerSystem;
			m_mousePickerSystem = 0;
		}

		if( m_globalHandleSystem )
		{
			delete m_globalHandleSystem;
			m_globalHandleSystem = 0;
		}

		if( m_scheduleManager != NULL )
		{
			delete m_scheduleManager;
			m_scheduleManager = NULL;
		}

		if( m_timingManager != NULL )
		{
			m_timingManager->removeAll(true);
			delete m_timingManager;
			m_timingManager = NULL;
		}

		if( m_eventManager != NULL )
		{
			delete m_eventManager;
			m_eventManager = NULL;
		}

		for( TVectorJoins::iterator
			it = m_joins.begin(),
			it_end = m_joins.end();
		it != it_end;
		++it )
		{
			delete (*it);
		}

		m_joins.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::initializeRenderResources()
	{
#	ifndef MENGE_MASTER_RELEASE
		ConstString c_debugText("debugText");

		m_debugText = NodeManager::get()->
			createNodeT<TextField>( c_debugText, Consts::get()->c_TextField, Consts::get()->c_builtin_empty );

		m_debugText->setResourceFont( Consts::get()->c_ConsoleFont );
		m_debugText->enable();
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::finalizeRenderResources()
	{
#	ifndef MENGE_MASTER_RELEASE
		if( m_debugText != NULL	 )
		{
			m_debugText->destroy();
			m_debugText = NULL;
		}
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handler = false;

		if( m_globalHandleSystem )
		{
			if( handler == false )
			{
				handler = m_globalHandleSystem->handleKeyEvent( _point, _key, _char, _isDown );
			}
		}

		if( m_arrow )
		{
			if( handler == false )
			{
				handler = m_mousePickerSystem->handleKeyEvent( _point, _key, _char, _isDown );
			}
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseButtonEvent( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handler = false;

		if( m_globalHandleSystem )
		{
			if( handler == false )
			{
				handler = m_globalHandleSystem->handleMouseButtonEvent( _point, _button, _isDown );
			}
		}

		if( m_arrow )
		{
			if( handler == false )
			{
				handler = m_mousePickerSystem->handleMouseButtonEvent( _point, _button, _isDown );
			}
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseButtonEventBegin( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		if( m_globalHandleSystem )
		{
			m_globalHandleSystem->handleMouseButtonEventBegin( _point, _button, _isDown );
		}

		if( m_arrow )
		{
			m_mousePickerSystem->handleMouseButtonEventBegin( _point, _button, _isDown );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseButtonEventEnd( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		if( m_globalHandleSystem )
		{
			m_globalHandleSystem->handleMouseButtonEventEnd( _point, _button, _isDown );
		}

		if( m_arrow )
		{
			m_mousePickerSystem->handleMouseButtonEventEnd( _point, _button, _isDown );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell )
	{
		//m_arrow->onMouseMove( _x, _y );

		bool handler = false;

		if( m_globalHandleSystem )
		{
			if( handler == false )
			{
				handler = m_globalHandleSystem->handleMouseMove( _point, _x, _y, _whell );
			}
		}

		if( m_arrow )
		{
			if( handler == false )
			{
				handler = m_mousePickerSystem->handleMouseMove( _point, _x, _y, _whell );
			}
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::tick( float _timing )
	{
		static float fpsTiming = 0.0f;
		fpsTiming += _timing;
		RenderEngine* re = Holder<RenderEngine>::get();
		if( fpsTiming >= 1000.0f )
		{
			m_fps = re->getDebugInfo().frameCount;
			re->resetFrameCount();
			while( fpsTiming >= 1000.0f ) 
			{
				fpsTiming -= 1000.0f;
			}
		}
		
		if( PhysicEngine2D::get()->isWorldCreate() )
		{
			const mt::vec2f & arrowPos = 
				m_arrow->getLocalPosition() + m_renderCamera2D->getViewport().begin;

			PhysicEngine2D::get()
				->onMouseMove( arrowPos );
		}

		if( m_renderCamera2D )
		{
			m_renderCamera2D->update( _timing );
		}

		if( m_arrow )
		{
			m_arrow->update( _timing );
		}

		if( m_mousePickerSystem )
		{
			m_mousePickerSystem->update();
		}

		if( m_scene )
		{
			m_scene->update( _timing );
		}

		this->updateJoins_();

		m_scheduleManager->update( _timing );
		m_timingManager->update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::update()
	{
		this->updateChangeScene_();

		if( m_globalHandleSystem )
		{
			m_globalHandleSystem->update();
		}

		if( m_eventManager )
		{
			m_eventManager->update();
		}			

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Join * Player::addJoin( Node * _left, Node * _right, const mt::vec2f & _offset )
	{
		Join * join = new Join(_left, _right, _offset);

		join->initialize();

		m_joins.push_back(join);

		//m_nodeJoins[_left].push_back(join);
		//m_nodeJoins[_right].push_back(join);
				
		return join;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::removeJoin( Join * _join )
	{
		TVectorJoins::iterator it_found = std::find( m_joins.begin(), m_joins.end(), _join );
		m_joins.erase( it_found );

		//Node * left = _join->getLeft();

		//TVectorJoins & leftJoins = m_nodeJoins[left];
		//TVectorJoins::iterator it_left_found = std::find( leftJoins.begin(), leftJoins.end(), _join );
		//leftJoins.erase( it_left_found );
		//
		//Node * right = _join->getRight();

		//TVectorJoins & rightJoins = m_nodeJoins[right];
		//TVectorJoins::iterator it_right_found = std::find( rightJoins.begin(), rightJoins.end(), _join );
		//rightJoins.erase( it_right_found );

		delete _join;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::isJoin( Node * _left, Node * _right )
	{
		for( TVectorJoins::iterator
			it = m_joins.begin(),
			it_end = m_joins.end();
		it != it_end;
		++it )
		{
			if( _left == (*it)->getLeft() && _right == (*it)->getRight() )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::getJoins( Node * _node, TVectorNode & _joins )
	{
		for( TVectorJoins::iterator
			it = m_joins.begin(),
			it_end = m_joins.end();
		it != it_end;
		++it )
		{
			Node * left = (*it)->getLeft();
			Node * right = (*it)->getRight();

			if( _node == left )
			{
				_joins.push_back( left );
			}
			else if( _node == right )
			{
				_joins.push_back( right );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::updateJoins_()
	{	
		//TVectorJoins join_updated;

		//for( TVectorJoins::iterator
		//	it = m_joins.begin(),
		//	it_end = m_joins.end();
		//it != it_end;
		//++it )
		//{
		//	Join * join = *it;

		//	if( join->valid() == true )
		//	{
		//		continue;
		//	}

		//	TVectorJoins::iterator it_found = std::find(join_updated.begin(), join_updated.end(), join);

		//	if( it_found != join_updated.end() )
		//	{
		//		continue;
		//	}
		//	
		//	TVectorJoins left_update;
		//	Node * left = join->getLeft()
		//	this->getJoins( left, left_update );

		//	TVectorJoins right_update;
		//	Node * right = join->getRight();
		//	this->getJoins( right, right_update );
		//		
		//	bool up = join->update();
		//	done |= up;
		//}

		bool done = true;
		size_t count = 10;

		while( done == true && --count )
		{
			done = false;

			for( TVectorJoins::iterator
				it = m_joins.begin(),
				it_end = m_joins.end();
			it != it_end;
			++it )
			{
				Join * join = *it;
				
				bool up = join->update();

				done |= up;
			}

			for( TVectorJoins::reverse_iterator
				it = m_joins.rbegin(),
				it_end = m_joins.rend();
			it != it_end;
			++it )
			{
				Join * join = *it;

				bool up = join->update();

				done |= up;
			}
		}

		//TVectorJoins::iterator it_first = m_joins.begin();

		//while(true)
		//{
		//	TVectorJoins::iterator it_next = 
		//		std::partition( it_first, m_joins.end(), std::mem_fun(&Join::update) );

		//	if( it_next == it_first )
		//	{
		//		break;
		//	}

		//	it_first = it_next;
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setRenderCamera2D( Camera2D * _camera)
	{
		m_renderCamera2D = _camera;
	}
	//////////////////////////////////////////////////////////////////////////
	Camera2D * Player::getRenderCamera2D()
	{
		return m_renderCamera2D;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setCamera2DPosition( const mt::vec2f & _pos )
	{
		if( m_renderCamera2D )
		{
			m_renderCamera2D->setLocalPosition( _pos );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	class VisitorPlayerFactoryManager
		: public VisitorFactoryManager
	{
	public:
		VisitorPlayerFactoryManager( std::stringstream & _ss )
			: m_ss(_ss)
		{
		}

	protected:
		void visit( const ConstString & _type, Factory * _factory ) override
		{
			std::size_t count = _factory->countObject();

			if( count == 0 )
			{
				return;
			}

			m_ss << "Factory Object " << Helper::to_str(_type) << ": " << count << "\n";
		}

	protected:
		std::stringstream & m_ss;
	};
	//////////////////////////////////////////////////////////////////////////
	void Player::render( unsigned int _debugMask )
	{
		if( m_switchScene == true )
		{
			return;
		}

		//if( m_scene == 0 )
		//{
		//	return;
		//}

		RenderEngine* renderEngine = RenderEngine::get();

		renderEngine->setCamera( m_renderCamera2D );

		const Viewport & renderport = m_renderCamera2D->getRenderport();
		const mt::mat3f & wm = m_renderCamera2D->getWorldMatrix();

		mt::mat3f inv_wm;

		mt::inv_m3(inv_wm, wm);

		renderEngine->newRenderPass( renderport, inv_wm );

		if( m_scene != NULL )
		{
			m_scene->render( m_renderCamera2D );
		}

		//renderEngine->setRenderArea( mt::vec4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

		renderEngine->beginLayer2D();
		renderEngine->setRenderTarget( Consts::get()->c_Window );

		mt::mat3f arrow_wm;
		mt::ident_m3(arrow_wm);

		renderEngine->newRenderPass( renderport, arrow_wm );

		if( m_arrow && m_arrow->hasParent() == false )
		{
			m_arrow->render( m_renderCamera2D );
		}

#	ifndef MENGE_MASTER_RELEASE
		if( m_showDebugText == true )
		{
			const RenderEngine::DebugInfo& redi = 
				RenderEngine::get()->getDebugInfo();

			//size_t particlesCount = 
			//	Holder<ParticleEngine>::get()->getFrameParticlesCount();

			size_t particlesCount = 0;

			std::stringstream ss;
			
			ss << "FPS: " << m_fps << "\n";
			ss << "DIP: " << redi.dips << "\n";
			ss << "Texture Memory Usage: " << (float)redi.textureMemory / (1024.f*1024.f) << "\n";
			ss << "Texture Count: " << redi.textureCount << "\n";
			ss << "Particles: " << particlesCount << "\n";
			ss << "Debug CRT:" << Application::get()->isDebugCRT() << "\n";

			VisitorPlayerFactoryManager pfmv(ss);

			NodeManager::get()
				->visitFactories( &pfmv );

			const std::string & str = ss.str();
			m_debugText->setText( str );
			m_debugText->render( m_renderCamera2D );
		}
#	endif

		renderEngine->endLayer2D();
		//m_renderCamera2D->setLocalPosition( pos );
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onAppMouseLeave()
	{
		if( m_arrow )
		{
			m_arrow->onAppMouseLeave();
		}

		if( m_scene && m_scene->isActivate() )
		{
			m_scene->onAppMouseLeave();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onAppMouseEnter()
	{
		if( m_arrow )
		{
			m_arrow->onAppMouseEnter();
		}

		if( m_scene && m_scene->isActivate() )
		{
			m_scene->onAppMouseEnter();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onFullscreen( const Resolution & _resolution, bool _fullscreen )
	{
		m_currentResolution = _resolution;

		if( m_renderCamera2D )
		{
			//mt::vec2f size;
			//size.x = float(m_contentResolution.getWidth());
			//size.y = float(m_contentResolution.getHeight());

			//m_renderCamera2D->setViewport( size );
			
			m_arrow->setCurrentResolution( m_currentResolution );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onFocus( bool _focus )
	{
		if( m_scene && m_scene->isActivate() )
		{
			m_scene->onFocus( _focus );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setCursorMode( bool _mode )
	{
		if( m_arrow != NULL )
		{
			m_arrow->setCursorMode( _mode );
		}
	}
	//////////////////////////////////////////////////////////////////////////
#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void Player::toggleDebugText()
	{
		m_showDebugText = !m_showDebugText;
	}
	//////////////////////////////////////////////////////////////////////////
#	endif
}
