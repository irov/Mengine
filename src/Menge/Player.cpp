#	include "Player.h"

#	include "Interface/RenderSystemInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/ResourceInterface.h"

#   include "Interface/EventInterface.h"
#	include "Interface/UnicodeInterface.h"

#	include "Interface/NotificationServiceInterface.h"
#	include "Interface/NotificatorInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/PrefetcherInterface.h"

#   include "Config/Stringstream.h"

#	include "NodeManager.h"

#	include "Game.h"
#	include "Application.h"

#	include "Kernel/Scene.h"
#	include "Kernel/Join.h"

#	include "RenderViewport.h"
#	include "Camera2D.h"
#	include "Consts.h"

#	include "Arrow.h"

#	include "Logger/Logger.h"

#	include "TextField.h"

#   include "Kernel/ResourceVisitor.h"
#   include "Kernel/ResourceReference.h"

#	include "Math/mat3.h"

#   include "pybind/pybind.hpp"

#   include <iomanip>

//#   include <stdex/allocator.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player( Game * _game )
        : m_game(_game)
        , m_serviceProvider(nullptr)
		, m_scene(nullptr)
		, m_arrow(nullptr)
		, m_scheduleManager(nullptr)
		, m_scheduleManagerGlobal(nullptr)
		, m_renderCamera(nullptr)
		, m_renderViewport(nullptr)
		, m_switchScene(false)
		, m_removeScene(false)
		, m_destroyOldScene(false)
		, m_restartScene(false)
		, m_arrowHided(false)
		, m_changeSceneCb(nullptr)
		, m_removeSceneCb(nullptr)
		, m_time(0.f)
		, m_fps(0)	
		, m_affectorable(nullptr)
		, m_affectorableGlobal(nullptr)
		, m_showDebugText(0)
		, m_debugText(nullptr)
		, m_camera2D(nullptr)
		, m_viewport2D(nullptr)
		, m_debugCamera2D(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Player::~Player()
	{
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
	bool Player::setCurrentScene( const ConstString& _name, bool _destroyOld, bool _destroyAfterSwitch, PyObject* _cb )
	{
		if( m_switchScene == true )
		{
			return false;
		}

		m_switchSceneName = _name;
		
		if( m_scene != nullptr && m_switchSceneName == m_scene->getName() )
		{
			m_restartScene = true;
		}

		m_switchScene = true;
        m_removeScene = false;

		m_destroyOldScene = _destroyOld;
		m_destroyAfterSwitch = _destroyAfterSwitch;

		if( _cb != nullptr && pybind::is_none(_cb) == false )
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

		if( _cb != nullptr && pybind::is_none(_cb) == false )
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
	void Player::updateChangeScene()
	{
		if( m_switchScene == true )
		{
			this->updateSwitchScene_();
		}
		else if( m_removeScene == true )
		{
			this->updateRemoveScene_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::updateRemoveScene_()
	{
		m_removeScene = false;

		if( m_scene != nullptr )
		{
			if( m_arrow != nullptr )
			{
				m_arrow->removeFromParent();
				m_arrow->disable();
			}

			m_scheduleManager->removeAll();
			m_timingManager->removeAll(false);
			m_affectorable->stopAllAffectors();

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
				Join * join = *it;

				delete join;
			}

			m_joins.clear();

			m_scene = nullptr;
		}

		if( m_mousePickerSystem != nullptr )
		{
			m_mousePickerSystem->setScene( nullptr );
		}

		if( m_removeSceneCb != nullptr )
		{
			PyObject * cb = m_removeSceneCb;
			m_removeSceneCb = nullptr;

			pybind::call( cb, "()" );
			pybind::decref( cb );
		}
	}
	//////////////////////////////////////////////////////////////////////////
    namespace
    {
        class PlayerResourceUselessCompile
        {
        public:
            PlayerResourceUselessCompile( ServiceProviderInterface * _serviceProvider )
                : m_serviceProvider(_serviceProvider)
            {
            }

            ~PlayerResourceUselessCompile()
            {
            }

        public:
            void begin()
            {
                m_observerResourceCompile = NOTIFICATION_SERVICE(m_serviceProvider)
                    ->addObserverMethod( NOTIFICATOR_RESOURCE_COMPILE, this, &PlayerResourceUselessCompile::resourceCompile );

                m_observerResourceRelease = NOTIFICATION_SERVICE(m_serviceProvider)
                    ->addObserverMethod( NOTIFICATOR_RESOURCE_RELEASE, this, &PlayerResourceUselessCompile::resourceRelease );
            }

            void end()
            {
                NOTIFICATION_SERVICE(m_serviceProvider)
                    ->removeObserver( NOTIFICATOR_RESOURCE_COMPILE, m_observerResourceCompile );

                NOTIFICATION_SERVICE(m_serviceProvider)
                    ->removeObserver( NOTIFICATOR_RESOURCE_RELEASE, m_observerResourceRelease );
            }

        protected:
            void resourceCompile( ResourceReference * _resource )
            {
                m_resources.push_back( _resource );
            }

            void resourceRelease( ResourceReference * _resource )
            {
                TVectorResourceDesc::iterator it_remove =
                    std::find( m_resources.begin(), m_resources.end(), _resource );

                if( it_remove == m_resources.end() )
                {
                    return;
                }
                
                LOGGER_ERROR(m_serviceProvider)("Useless Compile %s %s:%s"
					, _resource->getType().c_str()
                    , _resource->getCategory().c_str()
                    , _resource->getName().c_str()
                    );

                m_resources.erase( it_remove );
            }

        protected:
            ServiceProviderInterface * m_serviceProvider;

            Observer * m_observerResourceCompile;
            Observer * m_observerResourceRelease;

            typedef stdex::vector<ResourceReference *> TVectorResourceDesc;
            TVectorResourceDesc m_resources;
        };
    }
	//////////////////////////////////////////////////////////////////////////
	void Player::updateSwitchScene_()
	{
		m_switchScene = false;

		Scene * oldScene = m_scene;
		m_scene = nullptr;

		if( m_restartScene && oldScene != nullptr )		// just restart scene
		{		
			m_restartScene = false;

			m_switchSceneName = oldScene->getName();
		}

		if( m_arrow != nullptr )
		{
			m_arrow->removeFromParent();
			m_arrow->disable();
		}

		m_scheduleManager->removeAll();
		m_timingManager->removeAll(false);
		m_affectorable->stopAllAffectors();

		if( oldScene != nullptr && m_destroyOldScene == true && m_destroyAfterSwitch == false )
		{
            oldScene->destroy();
		}

		NODE_SERVICE(m_serviceProvider)
            ->clearHomeless();

		//m_globalHandleSystem->clear();

		if( oldScene != nullptr && m_destroyOldScene == true && m_destroyAfterSwitch == true )
		{
            oldScene->destroy();
		}

		for( TVectorJoins::iterator
			it = m_joins.begin(),
			it_end = m_joins.end();
		it != it_end;
		++it )
		{
			Join * join = *it;

			delete join;
		}

		m_joins.clear();

        PyObject * cb = m_changeSceneCb;
        m_changeSceneCb = nullptr;
        
        if( cb != nullptr )
        {
            pybind::call( cb, "(OO)", pybind::get_none(), pybind::get_bool(false) );
        }
        
        m_scene = PROTOTYPE_SERVICE(m_serviceProvider)
			->generatePrototypeT<Scene>( CONST_STRING(m_serviceProvider, Scene), m_switchSceneName );

		if( m_scene == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Player::updateChangeScene scene not found %s"
				, m_switchSceneName.c_str() 
				);

			if( cb != nullptr )
			{
				pybind::call( cb, "(OO)", pybind::get_none(), pybind::get_bool(false) );

				pybind::decref( cb );
			}

			return;
		}

		if( m_mousePickerSystem != nullptr )
		{
			m_mousePickerSystem->setScene( m_scene );
		}

		if( cb != nullptr )
		{
			pybind::call( cb, "(OO)", m_scene->getEmbed(), pybind::get_bool(false) );
		}

		//Holder<ResourceManager>::get()->_dumpResources( "before compile next scene " + m_scene->getName() );

#   ifndef MENGE_MASTER_RELEASE
        PlayerResourceUselessCompile unlessCompile(m_serviceProvider);
        unlessCompile.begin();
#   endif

        m_scene->enable();

#   ifndef MENGE_MASTER_RELEASE
        unlessCompile.end();
#   endif

        if( m_arrow != nullptr )
        {
            m_arrow->enable();
        }

		//Holder<ResourceManager>::get()->_dumpResources( "after compile next scene " + m_scene->getName() );

		if( cb != nullptr )
		{
			pybind::call( cb, "(OO)", m_scene->getEmbed(), pybind::get_bool(true) );
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
		if( m_arrow != nullptr )
		{
			m_arrow->disable();
		}

		m_arrow = _arrow;

		if( m_arrow != nullptr)
		{
			m_arrow->setRenderCamera( m_arrowCamera2D );
			m_arrow->setRenderViewport( m_renderViewport );

			if( m_scene != nullptr )
			{
				m_arrow->enable();
			}
		}

        if( m_mousePickerSystem != nullptr )
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
	void Player::calcGlobalMouseWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint )
	{
		const mt::mat4f & pm = m_renderCamera->getCameraProjectionMatrix();

		mt::mat4f pm_inv;
		mt::inv_m4( pm_inv, pm );

		mt::vec2f p1 = _screenPoint * 2.f - mt::vec2f(1.f, 1.f);
		p1.y = -p1.y;

		mt::vec2f p_pm;
		mt::mul_v2_m4( p_pm, p1, pm_inv );

		const mt::mat4f & vm = m_renderCamera->getCameraViewMatrix();

		mt::mat4f vm_inv;
		mt::inv_m4( vm_inv, vm );

		mt::vec2f p = p_pm;

		mt::vec2f p_vm;
		mt::mul_v2_m4( p_vm, p, vm_inv );

		_worldPoint = p_vm;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::calcGlobalMouseWorldDeltha( const mt::vec2f & _screenPoint, const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha )
	{
		const mt::mat4f & pm = m_renderCamera->getCameraProjectionMatrix();

		mt::mat4f pm_inv;
		mt::inv_m4( pm_inv, pm );

		mt::vec2f p1 = (_screenPoint + _screenDeltha) * 2.f - mt::vec2f(1.f, 1.f);
		p1.y = -p1.y;

		mt::vec2f p_pm;
		mt::mul_v2_m4( p_pm, p1, pm_inv );

		mt::vec2f p2 = (_screenPoint) * 2.f - mt::vec2f(1.f, 1.f);
		p2.y = -p2.y;

		mt::vec2f p_pm_base;
		mt::mul_v2_m4( p_pm_base, p2, pm_inv );

		mt::vec2f deltha = p_pm - p_pm_base;

		_worldDeltha = deltha;
	}
	//////////////////////////////////////////////////////////////////////////
	ScheduleManagerInterface * Player::createSchedulerManager()
	{
		ScheduleManagerInterface * sm = m_factoryScheduleManager.createObjectT();

		sm->initialize( m_serviceProvider );

		m_schedulers.push_back( sm );

		return sm;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::destroySchedulerManager( ScheduleManagerInterface * _scheduler )
	{	
		TVectorUserScheduler::iterator it_found = std::find( m_schedulers.begin(), m_schedulers.end(), _scheduler );

		if( it_found == m_schedulers.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("Player::destroySchedulerManager scheduler not found!"
				);

			return false;
		}
		
		m_schedulers.erase( it_found );

		_scheduler->destroy();

		return true;
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
	Affectorable * Player::getAffectorable() const
	{
		return m_affectorable;
	}
	//////////////////////////////////////////////////////////////////////////
	Affectorable * Player::getAffectorableGlobal() const
	{
		return m_affectorableGlobal;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::initialize( const Resolution & _contentResolution, const Resolution & _currentResolution )
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

		m_affectorable = new Affectorable;
		m_affectorableGlobal = new Affectorable;

		m_contentResolution = _contentResolution;
		m_currentResolution = _currentResolution;

		mt::vec2f cr;
		m_contentResolution.calcSize(cr);
		Viewport vp(0.f, 0.f, cr.x, cr.y);

		m_camera2D = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Camera2D>( CONST_STRING(m_serviceProvider, Camera2D) );

		m_camera2D->setRenderTarget( CONST_STRING(m_serviceProvider, Window) );
				
		m_camera2D->setRenderport( vp );

		//mt::vec2f vp_pos(crx * 0.5f, cry * 0.5f);
		//camera->setLocalPosition(vp_pos);

		m_camera2D->enable();
		this->setRenderCamera( m_camera2D );


		m_viewport2D = NODE_SERVICE(m_serviceProvider)
			->createNodeT<RenderViewport>( CONST_STRING(m_serviceProvider, RenderViewport) );
		
		m_viewport2D->setViewport( vp );
		m_viewport2D->enable();

		this->setRenderViewport( m_viewport2D );

		m_arrowCamera2D = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Camera2D>( CONST_STRING(m_serviceProvider, Camera2D) );

		m_arrowCamera2D->setRenderTarget( CONST_STRING(m_serviceProvider, Window) );
		m_arrowCamera2D->setRenderport( vp );
		m_arrowCamera2D->enable();
				
		m_debugCamera2D = NODE_SERVICE(m_serviceProvider)
			->createNodeT<Camera2D>( CONST_STRING(m_serviceProvider, Camera2D) );

		m_debugCamera2D->setRenderTarget( CONST_STRING(m_serviceProvider, Window) );

		m_debugCamera2D->setRenderport( vp );

		m_debugCamera2D->enable();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::finalize()
	{
        if( m_scene != nullptr )
        {
            m_scene->destroy();
            m_scene = nullptr;
        }

		pybind::decref(m_removeSceneCb);
		m_removeSceneCb = nullptr;
        
		pybind::decref(m_changeSceneCb);
		m_changeSceneCb = nullptr;
        
        if( m_camera2D != nullptr )
        {
            m_camera2D->destroy();
			m_camera2D = nullptr;
		}

		if( m_debugCamera2D != nullptr )
		{
			m_debugCamera2D->destroy();
			m_debugCamera2D = nullptr;
		}

		if( m_arrowCamera2D != nullptr )
		{
			m_arrowCamera2D->destroy();
			m_arrowCamera2D = nullptr;
		}

		if( m_viewport2D != nullptr )
		{
			m_viewport2D->destroy();
			m_viewport2D = nullptr;
		}

		if( m_mousePickerSystem != nullptr )
		{
			delete m_mousePickerSystem;
			m_mousePickerSystem = nullptr;
		}

		if( m_globalHandleSystem != nullptr )
		{
			delete m_globalHandleSystem;
			m_globalHandleSystem = nullptr;
		}

		if( m_scheduleManager != nullptr )
		{
			m_scheduleManager->destroy();
			m_scheduleManager = nullptr;
		}
		
		if( m_scheduleManagerGlobal != nullptr )
		{
			m_scheduleManagerGlobal->destroy();
			m_scheduleManagerGlobal = nullptr;
		}

		for( TVectorUserScheduler::iterator
			it = m_schedulers.begin(),
			it_end = m_schedulers.end();
		it != it_end;
		++it )
		{
			ScheduleManagerInterface * sm = *it;

			sm->destroy();
		}

		m_schedulers.clear();

		if( m_timingManager != nullptr )
		{
			m_timingManager->destroy();
			m_timingManager = nullptr;
		}

        if( m_timingManagerGlobal != nullptr )
        {
            m_timingManagerGlobal->destroy();
            m_timingManagerGlobal = nullptr;
        }

		if( m_affectorable != nullptr )
		{
			delete m_affectorable;
			m_affectorable = nullptr;
		}

		if( m_affectorableGlobal != nullptr )
		{
			delete m_affectorableGlobal;
			m_affectorableGlobal = nullptr;
		}

		for( TVectorJoins::iterator
			it = m_joins.begin(),
			it_end = m_joins.end();
		it != it_end;
		++it )
		{
			Join * join = *it;
			delete join;
		}

		m_joins.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::initializeRenderResources()
	{
		m_debugText = NODE_SERVICE(m_serviceProvider)->
			createNodeT<TextField>( CONST_STRING(m_serviceProvider, TextField) );

		m_debugText->setFontName( CONST_STRING_LOCAL(m_serviceProvider, "__CONSOLE_FONT__") );
		m_debugText->setTextID( CONST_STRING_LOCAL(m_serviceProvider, "__ID_TEXT_CONSOLE") );
		m_debugText->enable();
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::finalizeRenderResources()
	{
		if( m_debugText != nullptr	 )
		{
			m_debugText->destroy();
			m_debugText = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating )
	{
		bool handler = false;

		if( m_globalHandleSystem )
		{
			m_globalHandleSystem->handleGlobalKeyEvent( _point, _key, _char, _isDown, _repeating );			
		}

		if( m_mousePickerSystem )
		{
			if( handler == false )
			{
				handler = m_mousePickerSystem->handleKeyEvent( _point, _key, _char, _isDown, _repeating );
			}
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		if( m_globalHandleSystem )
		{
			m_globalHandleSystem->handleGlobalMouseButtonEvent( _touchId, _point, _button, _isDown );
		}

        bool handler = false;

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
			m_globalHandleSystem->handleGlobalMouseButtonEventBegin( _touchId, _point, _button, _isDown );
		}

        bool handler = false;

        if( m_mousePickerSystem )
        {
            if( handler == false )
            {
			    handler = m_mousePickerSystem->handleMouseButtonEventBegin( _touchId, _point, _button, _isDown );
            }
		}

        return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		if( m_globalHandleSystem )
		{
			m_globalHandleSystem->handleGlobalMouseButtonEventEnd( _touchId, _point, _button, _isDown );
		}

        bool handler = false;

        if( m_mousePickerSystem )
        {
            if( handler == false )
            {
                handler = m_mousePickerSystem->handleMouseButtonEventEnd( _touchId, _point, _button, _isDown );
            }
		}

        return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y )
	{
		if( m_globalHandleSystem )
		{
			m_globalHandleSystem->handleGlobalMouseMove( _touchId, _point, _x, _y );
		}

        bool handler = false;

		if( m_mousePickerSystem )
		{
			if( handler == false )
			{
				handler = m_mousePickerSystem->handleMouseMove( _touchId, _point, _x, _y );
			}
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel )
	{
		if( m_globalHandleSystem )
		{
			m_globalHandleSystem->handleGlobalMouseWheel( _touchId, _point, _wheel );
		}

		bool handler = false;

		if( m_mousePickerSystem )
		{
			if( handler == false )
			{
				handler = m_mousePickerSystem->handleMouseWheel( _touchId, _point, _wheel );
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

		if( m_camera2D != nullptr )
		{
			m_camera2D->update( gameTime, _timing );
		}

		if( m_arrowCamera2D != nullptr )
		{
			m_arrowCamera2D->update( gameTime, _timing );
		}

		if( m_arrow != nullptr )
		{
			m_arrow->update( gameTime, _timing );
		}

		if( m_scene != nullptr )
		{
			m_scene->update( gameTime, _timing );
		}

		this->updateJoins_();

		if( m_scheduleManager != nullptr )
		{
			m_scheduleManager->update( gameTime, _timing );
		}

		if( m_scheduleManagerGlobal != nullptr )
		{
			m_scheduleManagerGlobal->update( gameTime, _timing );
		}

		for( TVectorUserScheduler::iterator
			it = m_schedulers.begin(),
			it_end = m_schedulers.end();
		it != it_end;
		++it )
		{
			ScheduleManagerInterface * sm = *it;

			sm->update( gameTime, _timing );
		}

		if( m_affectorable != nullptr )
		{
			m_affectorable->updateAffectors( gameTime, _timing );
		}

		if( m_affectorableGlobal != nullptr )
		{
			m_affectorableGlobal->updateAffectors( gameTime, _timing );
		}

		m_timingManager->update( gameTime, _timing );
        m_timingManagerGlobal->update( gameTime, _timing );

		MODULE_SERVICE(m_serviceProvider)
			->update( gameTime, _timing );

		m_time += _timing;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::update()
	{
		if( m_mousePickerSystem != nullptr )
		{
			m_mousePickerSystem->update();
		}

		if( m_globalHandleSystem != nullptr )
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
		uint32_t count = 10;

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
	void Player::setRenderCamera( RenderCameraInterface * _camera)
	{
		m_renderCamera = _camera;

		if( m_mousePickerSystem != nullptr )
		{
			m_mousePickerSystem->setRenderCamera( m_renderCamera );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderCameraInterface * Player::getRenderCamera() const
	{
		return m_renderCamera;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setRenderViewport( RenderViewportInterface * _renderViewport )
	{
		m_renderViewport = _renderViewport;

		if( m_mousePickerSystem != nullptr )
		{
			m_mousePickerSystem->setRenderViewport( m_renderViewport );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderViewportInterface * Player::getRenderViewport() const 
	{
		return m_renderViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	float Player::getTime() const
	{
		return m_time;
	}
	//////////////////////////////////////////////////////////////////////////
	class VisitorPlayerFactoryManager
		: public VisitorPrototypeGenerator
	{
	public:
		VisitorPlayerFactoryManager( ServiceProviderInterface * _serviceProvider, const ConstString & _category, Stringstream & _ss )
			: m_serviceProvider(_serviceProvider)
            , m_category(_category)
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
		void visit( const ConstString & _category, const ConstString & _type, const PrototypeGeneratorInterfacePtr & _generator ) override
		{
            if( m_category != _category )
            {
                return;
            }

			uint32_t count = _generator->count();

			if( count == 0 )
			{
				return;
			}

			m_ss << "Factory Object " << _type.c_str() << ": " << count << "\n";
		}

	protected:
        ServiceProviderInterface * m_serviceProvider;
        ConstString m_category;
		Stringstream & m_ss;
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

		if( m_scene != nullptr )
		{
			m_scene->render( m_renderViewport, m_renderCamera, debugMask );
		}

		//renderEngine->setRenderArea( mt::vec4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

		//mt::mat4f arrow_wm;
		//mt::ident_m4(arrow_wm);

		//const mt::mat4f & arrow_vm = m_renderCamera2D->getViewMatrix();
		//const mt::mat4f & arrow_pm = m_renderCamera2D->getProjectionMatrix();

		//renderEngine->newRenderPass( m_renderCamera2D );

		MODULE_SERVICE(m_serviceProvider)
			->render( m_renderViewport, m_renderCamera, debugMask );

		if( m_arrow && m_arrow->hasParent() == false )
		{
			m_arrow->render( m_renderViewport, m_arrowCamera2D, debugMask );
		}

//#	ifndef MENGE_MASTER_RELEASE

		if( m_showDebugText != 0 )
		{			
			const RenderDebugInfo & rdi = 
				RENDER_SERVICE(m_serviceProvider)->getDebugInfo();

			const RenderTextureDebugInfo & rtdi =
				RENDERTEXTURE_SERVICE(m_serviceProvider)->getDebugInfo();

			//size_t particlesCount = 
			//	Holder<ParticleEngine>::get()->getFrameParticlesCount();

			uint32_t particlesCount = 0;

			Stringstream ss;

			ss << "FPS: " << m_fps << std::endl;

			if( m_showDebugText > 1 )
			{
				double sreenfillrate = rdi.fillrate / double(m_contentResolution.getWidth() * m_contentResolution.getHeight());

				ss << "Fillrate " << std::setiosflags(std::ios::fixed) << std::setprecision(2) << sreenfillrate << " (Object " << rdi.object << " Triangle " << rdi.triangle << ")" << std::endl;
				ss << "DIP: " << rdi.dips << std::endl;

				ERenderBatchMode mode = RENDER_SERVICE(m_serviceProvider)
					->getBatchMode();

				ss << "Smart Batch: " << mode << " " << rdi.batch <<  std::endl;
				ss << "Texture Memory Usage: " << (float)rtdi.textureMemory / (1024.f*1024.f) << std::endl;
				ss << "Texture Count: " << rtdi.textureCount << std::endl;
				ss << "Particles: " << particlesCount << std::endl;
			}

			if( m_showDebugText == 2 )
			{
				class CompileResourceVisitor
					: public ResourceVisitor
				{
				public:
					CompileResourceVisitor()
						: m_count(0)
					{
					}

				public:
					uint32_t getCount() const
					{
						return m_count;
					}

				protected:
					void visit( ResourceReference * _resource )
					{
						if( _resource->isCompile() == false )
						{
							return;
						}

						++m_count;
					}

				protected:
					uint32_t m_count;
				};

				CompileResourceVisitor crv;

				RESOURCE_SERVICE(m_serviceProvider)
					->visitResources( &crv );

				ss << "Resources: " << crv.getCount() << std::endl;

				PrefetcherDebugInfo pdInfo = PREFETCHER_SERVICE(m_serviceProvider)
					->getDebugInfo();

				ss << "Prefetcher: decoder " << pdInfo.decoderCount << std::endl;
				ss << "Prefetcher: data " << pdInfo.dataCount << std::endl;

				MousePickerSystemInterface * mousePickerSystem = 
					PLAYER_SERVICE(m_serviceProvider)->getMousePickerSystem();

				ss << "PickerTrapCount:" << mousePickerSystem->getPickerTrapCount() << std::endl;
			}
			else if( m_showDebugText == 3 )
            {
			    VisitorPlayerFactoryManager pfmv(m_serviceProvider, CONST_STRING(m_serviceProvider, Node), ss);

			    PROTOTYPE_SERVICE(m_serviceProvider)
				    ->visitGenerators( &pfmv );
            }
			else if( m_showDebugText == 4 )
            {
                class MyVisitorClassTypeScope
                    : public pybind::visitor_class_type_scope
                {
                public:
                    MyVisitorClassTypeScope( Stringstream & _ss )
                        : m_ss(_ss)
                        , m_count(0)
                    {
                    }

                protected:
                    void operator = ( const MyVisitorClassTypeScope & )
                    {
                    }

                protected:
                    void visit_scope( pybind::class_type_scope * _scope ) override
                    {   
                        uint32_t count = _scope->getObjectCount();

                        if( count == 0 )
                        {
                            return;
                        }

                        if( m_count % 3 == 0 )
                        {
                            m_ss << "Py: ";
                        }
                        
                        m_ss << _scope->get_name() << " " << _scope->getObjectCount();
                        
                        if( m_count % 3 != 2 )
                        {
                            m_ss << "        ";
                        }
                        else
                        {
                            m_ss << std::endl;
                        }                        

                        ++m_count;
                    }

                protected:                    
                    Stringstream & m_ss;
                    uint32_t m_count;
                };

                MyVisitorClassTypeScope mvcts(ss);
                pybind::visit_class_type_scope(&mvcts);
            }
            
			String text = ss.str();

			TVectorString args;
			args.push_back( text );
			m_debugText->setTextFormatArgs( args );

            float gameViewportAspect;
            Viewport gameViewport;

            APPLICATION_SERVICE(m_serviceProvider)
                ->getGameViewport( gameViewportAspect, gameViewport );

            m_debugText->setLocalPosition( mt::vec3f(gameViewport.begin, 0.f) );
			m_debugText->render( m_renderViewport, m_debugCamera2D, debugMask );
		}
//#	endif
		//m_renderCamera2D->setLocalPosition( pos );
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onAppMouseLeave()
	{
		if( m_arrow != nullptr )
		{
			m_arrow->onAppMouseLeave();
		}

		if( m_scene != nullptr && m_scene->isActivate() == true )
		{
			m_scene->onAppMouseLeave();
		}

		if( m_mousePickerSystem )
		{
			m_mousePickerSystem->handleMouseLeave();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onAppMouseEnter()
	{
		if( m_arrow != nullptr )
		{
			m_arrow->onAppMouseEnter();
		}

		if( m_scene != nullptr && m_scene->isActivate() == true )
		{
			m_scene->onAppMouseEnter();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onAppMousePosition( const mt::vec2f & _point )
	{
		if( m_mousePickerSystem != nullptr )
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
    }
	//////////////////////////////////////////////////////////////////////////
	void Player::onFocus( bool _focus )
	{
		if( m_scene != nullptr && m_scene->isActivate() == true )
		{
			m_scene->onFocus( _focus );
		}
	}
	//////////////////////////////////////////////////////////////////////////
//#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void Player::toggleDebugText()
	{
		++m_showDebugText;
        m_showDebugText %= 4;

        RENDER_SERVICE(m_serviceProvider)
            ->enableDebugMode( m_showDebugText > 1 );
	}
	//////////////////////////////////////////////////////////////////////////
//#	endif
}
