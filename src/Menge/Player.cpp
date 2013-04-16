#	include "Player.h"

#	include "NodeManager.h"

#	include "Game.h"
#	include "Application.h"

#	include "Kernel/Scene.h"
#	include "Kernel/Join.h"

#	include "Camera2D.h"
#	include "Consts.h"

#	include "Arrow.h"

#	include "Logger/Logger.h"

#	include "TextField.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/PhysicSystem2DInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"

#   include "Interface/SceneInterface.h"
#   include "Interface/EventInterface.h"
#	include "Interface/UnicodeInterface.h"

#   include "MousePickerSystem.h"
#   include "GlobalHandleSystem.h"
#   include "ScheduleManager.h"
#   include "TimingManager.h"

#	include "Math/mat3.h"

#   include "pybind/pybind.hpp"

#	include <sstream>
#   include <iomanip>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player( Game * _game )
        : m_game(_game)
        , m_serviceProvider(NULL)
		, m_scene(NULL)
		, m_arrow(NULL)
		, m_scheduleManager(NULL)
		, m_scheduleManagerGlobal(NULL)
		, m_renderCamera2D(0)
		, m_switchScene(false)
		, m_removeScene(false)
		, m_destroyOldScene(false)
		, m_restartScene(false)
		, m_arrowHided(false)
		, m_changeSceneCb(NULL)
		, m_removeSceneCb(NULL)
		, m_time(0.f)
		, m_fps(0)	
//#	ifndef MENGE_MASTER_RELEASE
		, m_showDebugText(false)
		, m_debugText(NULL)
//#	endif
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
        m_removeScene = false;

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

        m_switchScene = false;
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

            NODE_SERVICE(m_serviceProvider)
                ->clearHomeless();

            m_scene->release();

			for( TVectorJoins::iterator
				it = m_joins.begin(),
				it_end = m_joins.end();
			it != it_end;
			++it )
			{
				delete (*it);
			}

			m_joins.clear();

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

		if( m_restartScene && oldScene != NULL )		// just restart scene
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

		if( oldScene != NULL && m_destroyOldScene == true && m_destroyAfterSwitch == false )
		{
            oldScene->destroy();
		}

		NODE_SERVICE(m_serviceProvider)
            ->clearHomeless();

		m_mousePickerSystem->clear();
		m_globalHandleSystem->clear();

		if( oldScene != NULL && m_destroyOldScene == true && m_destroyAfterSwitch == true )
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

        PyObject * cb = m_changeSceneCb;
        m_changeSceneCb = NULL;
        
        if( cb != NULL )
        {
            pybind::call( cb, "(OO)", pybind::get_none(), pybind::get_bool(false) );
        }
        
        m_scene = SCENE_SERVICE(m_serviceProvider)
			->createScene( m_switchSceneName );

		if( m_scene == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "Player::updateChangeScene scene not found %s"
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

        if( m_arrow != NULL )
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

        m_switchSceneName.clear();

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

        if( m_mousePickerSystem )
        {
		    m_mousePickerSystem->setArrow( m_arrow );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * Player::getArrow() const
	{
		return m_arrow;
	}
    //////////////////////////////////////////////////////////////////////////
    MousePickerSystemInterface * Player::getMousePickerSystem() const
    {
        return m_mousePickerSystem;
    }
    //////////////////////////////////////////////////////////////////////////
    GlobalHandleSystemInterface * Player::getGlobalHandleSystem() const
    {
        return m_globalHandleSystem;
    }
	//////////////////////////////////////////////////////////////////////////
	ScheduleManagerInterface * Player::getScheduleManager() const
	{
		return m_scheduleManager;
	}
	//////////////////////////////////////////////////////////////////////////
	ScheduleManagerInterface * Player::getScheduleManagerGlobal() const
	{
		return m_scheduleManagerGlobal;
	}	
	//////////////////////////////////////////////////////////////////////////
	TimingManagerInterface * Player::getTimingManager() const
	{
		return m_timingManager;
	}
    //////////////////////////////////////////////////////////////////////////
    TimingManagerInterface * Player::getTimingManagerGlobal() const
    {
        return m_timingManagerGlobal;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Player::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Player::initialize( Arrow * _arrow, const Resolution & _contentResolution, const Resolution & _currentResolution )
	{
		m_mousePickerSystem = new MousePickerSystem(m_serviceProvider);
		m_globalHandleSystem = new GlobalHandleSystem(m_serviceProvider);
		
        m_scheduleManager = m_factoryScheduleManager.createObjectT();
        m_scheduleManager->initialize( m_serviceProvider );

		m_scheduleManagerGlobal = m_factoryScheduleManager.createObjectT();
        m_scheduleManagerGlobal->initialize( m_serviceProvider );

		m_timingManager = m_factoryTimingManager.createObjectT();
        m_timingManager->initialize(m_serviceProvider);

        m_timingManagerGlobal = m_factoryTimingManager.createObjectT();
        m_timingManager->initialize(m_serviceProvider);

		m_contentResolution = _contentResolution;
		m_currentResolution = _currentResolution;

		if( _arrow == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("Player::init default arrow not found"
                );

			return false;
		}

		float crx = float( m_contentResolution.getWidth() );
		float cry = float( m_contentResolution.getHeight() );

		Camera2D * camera = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Camera2D>( CONST_STRING(m_serviceProvider, Camera2D) );

		camera->setRenderTarget( CONST_STRING(m_serviceProvider, Window) );

		Viewport vp(0.f, 0.f, crx, cry);
		camera->setViewport( vp );

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
			m_scheduleManager->destroy();
			m_scheduleManager = NULL;
		}

		if( m_scheduleManagerGlobal != NULL )
		{
			m_scheduleManagerGlobal->destroy();
			m_scheduleManagerGlobal = NULL;
		}

		if( m_timingManager != NULL )
		{
			m_timingManager->destroy();
			m_timingManager = NULL;
		}

        if( m_timingManagerGlobal != NULL )
        {
            m_timingManagerGlobal->destroy();
            m_timingManagerGlobal = NULL;
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
//#	ifndef MENGE_MASTER_RELEASE
		m_debugText = NODE_SERVICE(m_serviceProvider)->
			createNodeT<TextField>( CONST_STRING(m_serviceProvider, TextField) );

		m_debugText->setResourceFont( CONST_STRING(m_serviceProvider, ConsoleFont) );
		m_debugText->enable();
//#	endif
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

		if( m_mousePickerSystem )
		{
			if( handler == false )
			{
				handler = m_mousePickerSystem->handleKeyEvent( _point, _key, _char, _isDown );
			}
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handler = false;

		if( m_globalHandleSystem )
		{
			if( handler == false )
			{
				handler = m_globalHandleSystem->handleMouseButtonEvent( _touchId, _point, _button, _isDown );
			}
		}

		if( m_mousePickerSystem )
		{
			if( handler == false )
			{
				handler = m_mousePickerSystem->handleMouseButtonEvent( _touchId, _point, _button, _isDown );
			}
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		if( m_globalHandleSystem )
		{
			m_globalHandleSystem->handleMouseButtonEventBegin( _touchId, _point, _button, _isDown );
		}

		if( m_mousePickerSystem )
		{
			m_mousePickerSystem->handleMouseButtonEventBegin( _touchId, _point, _button, _isDown );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		if( m_globalHandleSystem )
		{
			m_globalHandleSystem->handleMouseButtonEventEnd( _touchId, _point, _button, _isDown );
		}

		if( m_mousePickerSystem )
		{
			m_mousePickerSystem->handleMouseButtonEventEnd( _touchId, _point, _button, _isDown );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell )
	{
		//m_arrow->onMouseMove( _x, _y );

		bool handler = false;

		if( m_globalHandleSystem )
		{
			if( handler == false )
			{
				handler = m_globalHandleSystem->handleMouseMove( _touchId, _point, _x, _y, _whell );
			}
		}

		if( m_mousePickerSystem )
		{
			if( handler == false )
			{
				handler = m_mousePickerSystem->handleMouseMove( _touchId, _point, _x, _y, _whell );
			}
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::tick( float _timing )
	{
		static float fpsTiming = 0.0f;
		fpsTiming += _timing;
		if( fpsTiming >= 1000.0f )
		{
			m_fps = RENDER_SERVICE(m_serviceProvider)->getDebugInfo().frameCount;
			RENDER_SERVICE(m_serviceProvider)->resetFrameCount();
			while( fpsTiming >= 1000.0f ) 
			{
				fpsTiming -= 1000.0f;
			}
		}
		
		float gameTime = m_time;
		
		//if( PhysicEngine2D::get()->isWorldCreate() )
		//{
		//	const mt::vec2f & arrowPos = 
		//		m_arrow->getLocalPosition() + m_renderCamera2D->getViewport().begin;

		//	PhysicEngine2D::get()
		//		->onMouseMove( arrowPos );
		//}

		if( m_renderCamera2D )
		{
			m_renderCamera2D->update( gameTime, _timing );
		}

		if( m_arrow )
		{
			m_arrow->update( gameTime, _timing );
		}

		if( m_scene )
		{
			m_scene->update( gameTime, _timing );
		}

		this->updateJoins_();

		if( m_scheduleManager )
		{
			m_scheduleManager->update( gameTime, _timing );
		}

		if( m_scheduleManagerGlobal )
		{
			m_scheduleManagerGlobal->update( gameTime, _timing );
		}

		m_timingManager->update( gameTime, _timing );
        m_timingManagerGlobal->update( gameTime, _timing );

		m_time += _timing;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::update()
	{
		this->updateChangeScene_();

		if( m_mousePickerSystem )
		{
			m_mousePickerSystem->beginTrap();
		}		

		if( m_mousePickerSystem )
		{
			m_mousePickerSystem->update();
		}

		if( m_globalHandleSystem )
		{
			m_globalHandleSystem->update();
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
	bool Player::isJoin( Node * _left, Node * _right ) const
	{
		for( TVectorJoins::const_iterator
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
	void Player::getJoins( Node * _node, TVectorNode & _joins ) const
	{
		for( TVectorJoins::const_iterator
			it = m_joins.begin(),
			it_end = m_joins.end();
		it != it_end;
		++it )
		{
			Node * left = (*it)->getLeft();
			Node * right = (*it)->getRight();

			if( _node == left )
			{
				_joins.push_back( right );
			}
			else if( _node == right )
			{
				_joins.push_back( left );
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
	Camera2D * Player::getRenderCamera2D() const
	{
		return m_renderCamera2D;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setCamera2DPosition( const mt::vec2f & _pos )
	{
		if( m_renderCamera2D )
		{
			mt::vec3f v3_pos(_pos, 0.f);
			m_renderCamera2D->setLocalPosition( v3_pos );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float Player::getTime() const
	{
		return m_time;
	}
	//////////////////////////////////////////////////////////////////////////
	class VisitorPlayerFactoryManager
		: public VisitorFactoryManager
	{
	public:
		VisitorPlayerFactoryManager( ServiceProviderInterface * _serviceProvider, WStringstream & _ss )
			: m_serviceProvider(_serviceProvider)
            , m_ss(_ss)
		{
		}

    private:
        VisitorPlayerFactoryManager & operator = ( const VisitorPlayerFactoryManager & _vpfm )
        {
            (void)_vpfm;

            return *this;
        }

	protected:
		void visit( const ConstString & _type, Factory * _factory ) override
		{
			size_t count = _factory->countObject();

			if( count == 0 )
			{
				return;
			}

            String str_type = _type.c_str();

            WString unicode_type;
            if( Helper::utf8ToUnicode( m_serviceProvider, str_type, unicode_type ) == false )
            {
                return;
            }

			m_ss << L"Factory Object " << unicode_type << L": " << count << L"\n";
		}

	protected:
        ServiceProviderInterface * m_serviceProvider;
		WStringstream & m_ss;
	};
	//////////////////////////////////////////////////////////////////////////
	void Player::render()
	{
		if( m_switchScene == true )
		{
			return;
		}

		//if( m_scene == 0 )
		//{
		//	return;
		//}

		//const Viewport & vp = m_renderCamera2D->getViewport();
		//const mt::mat4f & wm = m_renderCamera2D->getWorldMatrix();

		//mt::mat4f inv_wm;

		//mt::inv_m4(inv_wm, wm);

		//const mt::mat4f & camera_vm = m_renderCamera2D->getViewMatrix();
		//const mt::mat4f & camera_pm = m_renderCamera2D->getProjectionMatrix();


		//renderEngine->newRenderPass( m_renderCamera2D );

        unsigned int debugMask = APPLICATION_SERVICE(m_serviceProvider)
            ->getDebugMask();

		if( m_scene != NULL )
		{
			m_scene->render( m_renderCamera2D, debugMask );
		}

		//renderEngine->setRenderArea( mt::vec4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

		//mt::mat4f arrow_wm;
		//mt::ident_m4(arrow_wm);

		//const mt::mat4f & arrow_vm = m_renderCamera2D->getViewMatrix();
		//const mt::mat4f & arrow_pm = m_renderCamera2D->getProjectionMatrix();

		//renderEngine->newRenderPass( m_renderCamera2D );

		if( m_arrow && m_arrow->hasParent() == false )
		{
			m_arrow->render( m_renderCamera2D, debugMask );
		}

//#	ifndef MENGE_MASTER_RELEASE
		if( m_showDebugText == true )
		{
			const RenderDebugInfo & redi = 
				RENDER_SERVICE(m_serviceProvider)->getDebugInfo();

			//size_t particlesCount = 
			//	Holder<ParticleEngine>::get()->getFrameParticlesCount();

			size_t particlesCount = 0;

			WStringstream ss;
			
			ss << L"FPS: " << m_fps << std::endl;

            double sreenfillrate = redi.fillrate / double(m_contentResolution.getWidth() * m_contentResolution.getHeight());

            ss << L"Fillrate " << std::setiosflags(std::ios::fixed) << std::setprecision(2) << sreenfillrate << " (Object " << redi.object << " Triangle " << redi.triangle << ")" << std::endl;
			ss << L"DIP: " << redi.dips << std::endl;
			ss << L"Texture Memory Usage: " << (float)redi.textureMemory / (1024.f*1024.f) << std::endl;
			ss << L"Texture Count: " << redi.textureCount << std::endl;
			ss << L"Particles: " << particlesCount << std::endl;

            MousePickerSystemInterface * mousePickerSystem = 
                PLAYER_SERVICE(m_serviceProvider)->getMousePickerSystem();

			ss << L"PickerTrapCount:" << mousePickerSystem->getPickerTrapCount() << std::endl;

			VisitorPlayerFactoryManager pfmv(m_serviceProvider, ss);

			NODE_SERVICE(m_serviceProvider)
				->visitFactories( &pfmv );

            ss << L"Entities: " << Entity::s_enum << std::endl;

			WString text = ss.str();

			m_debugText->setText( text );
			m_debugText->render( m_renderCamera2D, debugMask );
		}
//#	endif
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

		if( m_mousePickerSystem )
		{
			m_mousePickerSystem->handleMouseLeave();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onAppMouseEnter( const mt::vec2f & _point )
	{
		if( m_arrow )
		{
			m_arrow->onAppMouseEnter();
		}

		if( m_scene && m_scene->isActivate() )
		{
			m_scene->onAppMouseEnter();
		}

		if( m_mousePickerSystem )
		{
			m_mousePickerSystem->handleMouseEnter( _point );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onFullscreen( const Resolution & _resolution, bool _fullscreen )
	{
        (void)_fullscreen;

		this->invalidateResolution_( _resolution );
	}
    //////////////////////////////////////////////////////////////////////////
    void Player::onFixedContentResolution( const Resolution & _resolution, bool _fixed )
    {
        (void)_fixed;

        this->invalidateResolution_( _resolution );
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::invalidateResolution_( const Resolution & _resolution )
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
//#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void Player::toggleDebugText()
	{
		m_showDebugText = !m_showDebugText;

        RENDER_SERVICE(m_serviceProvider)
            ->enableDebugMode( m_showDebugText );
	}
	//////////////////////////////////////////////////////////////////////////
//#	endif
}
