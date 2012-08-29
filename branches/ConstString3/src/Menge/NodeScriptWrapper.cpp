
#	include "ScriptWrapper.h"

#	include "InputEngine.h"
#	include "EntityManager.h"
#	include "NodeManager.h"
#	include "FileEngine.h"
#	include "Scene.h"
#	include "Game.h"

#	include "Consts.h"

#	include "ScriptClassWrapper.h"

#	include "ScriptEngine.h"
#	include "ScheduleManager.h"

#	include "TextManager.h"
#	include "ArrowManager.h"
#	include "SceneManager.h"
#	include "AccountManager.h"

#	include "EventManager.h"

#	include "ResourceManager.h"
#	include "ResourceMovie.h"
#	include "ResourceImageDynamic.h"
#	include "ResourceImageDefault.h"
#	include "ResourceHotspotImage.h"
#	include "ResourceEmitterContainer.h"

#	include "Player.h"
#	include "Application.h"
#	include "MousePickerSystem.h"

#	include "Sprite.h"
#	include "MovieInternalObject.h"
#	include "Animation.h"
#	include "HotSpot.h"
#	include "Light2D.h"
#	include "ShadowCaster2D.h"
#	include "Arrow.h"
#	include "TextField.h"
#	include "SoundEmitter.h"
#	include "ParticleEmitter.h"
#	include "Point.h"
#	include "Camera3d.h"
//#	include "RigidBody3D.h"
//#	include "CapsuleController.h"
#	include "Skeleton.h"
#	include "Camera2D.h"

#	include "Layer2D.h"
#	include "Layer2DPhysic.h"
#	include "Layer2DTexture.h"

//#	include "RigidBody2D.h"

#	include "PhysicalBody2D.h"
#	include "TilePolygon.h"

#	include "Movie.h"
#	include "Video.h"

#	include "Window.h"
#	include "HotSpotImage.h"

#	include "Entity.h"

//#	include "DiscreteEntity.h"

//#	include "SoundEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"

#	include "TimingManager.h"

#	include "Identity.h"

#	include "Affector.h"

#	include "Camera3D.h"
#	include "RenderMesh.h"

#	include "ResourceVisitor.h"

#   include "ThreadTask.h"
#	include "ThreadTaskManager.h"
#	include "ThreadTaskDeferredLoading.h"
#	include "ThreadTaskLoadResourceImage.h"
#	include "ThreadTaskLoadImageResources.h"

#	include "Core/Polygon.h"

#	include "Utils/Math/angle.h"
#	include "Utils/Math/vec4.h"
#	include "Utils/Math/mat3.h"
#	include "Utils/Math/mat4.h"
#	include "Utils/Math/quat.h"
#	include "Utils/Math/clamp.h"

#	include "Utils/Core/Rect.h"
#	include "Utils/Core/String.h"

#	include "Join.h"

#	include <sstream>

#	include <boost/geometry/geometry.hpp> 
#	include <boost/geometry/geometries/point_xy.hpp>
#	include <boost/geometry/geometries/polygon.hpp>
#	include <boost/geometry/algorithms/intersects.hpp>

namespace Menge
{
	namespace ScriptMethod
	{
		namespace HotSpotAdapter
		{
			static mt::vec2f s_getLocalPolygonCenter( HotSpot * _hs )
			{
				mt::vec2f pc(0.f, 0.f);

				const Polygon & polygon = _hs->getPolygon();

				const Polygon::ring_type & ring = polygon.outer();

				if( ring.empty() == true )
				{
					return mt::vec2f(0.f, 0.f);
				}

				for( Polygon::ring_type::const_iterator 
					it = ring.begin(),
					it_end = ring.end();
				it != it_end;
				++it )
				{
					pc += *it;
				}

				pc -= ring.back();

				float size = (float)boost::geometry::num_points(polygon);
				pc /= (size - 1);

				return pc;
			}
			
			static mt::vec2f s_getWorldPolygonCenter( HotSpot * _hs )
			{
				mt::vec2f pc = s_getLocalPolygonCenter( _hs );

				const mt::mat4f & wm = _hs->getWorldMatrix();

				mt::vec2f world_pc;
				mt::mul_v2_m4(world_pc, pc, wm);

				return world_pc;
			}
			
			static Polygon s_getWorldPolygon( HotSpot * _hs )
			{
				const Polygon & polygon = _hs->getPolygon();

				const mt::mat4f & wm = _hs->getWorldMatrix();

				Polygon pwm;
				polygon_wm(pwm, polygon, wm);

				return pwm;
			}
		}

		static Polygon s_polygon_wm( const Polygon & _polygon, const mt::mat4f & _wm )
		{
			Polygon polygon;
			polygon_wm(polygon, _polygon, _wm);

			return polygon;
		}

		static bool s_testHotspot( HotSpot * _left, HotSpot * _right )
		{
			const Polygon & left_poligon = _left->getPolygon();
			const mt::mat4f & left_wm = _left->getWorldMatrix();

			const Polygon & right_poligon = _right->getPolygon();
			const mt::mat4f & right_wm = _right->getWorldMatrix();
			
			Polygon left_polygon_wm;

			polygon_wm(left_polygon_wm, left_poligon, left_wm);

			Polygon right_polygon_wm;

			polygon_wm(right_polygon_wm, right_poligon, right_wm);

			bool result = boost::geometry::intersects( left_polygon_wm, right_polygon_wm );
			
			return result;
			
			//return _left->testPolygon( left_wm, right_poligon, right_wm );
		}

		static bool s_loadPlugin( const WString & _pluginName )
		{
			bool result = Application::get()
				->loadPlugin( _pluginName );

			return result;
		}

		static void s_blockInput( bool _value )
		{
			MousePickerSystem::get()
				->block(_value);
		}

		static void s_setInputMouseButtonEventBlock( bool _value )
		{
			Application::get()
				->setInputMouseButtonEventBlock( _value );
		}

		static bool s_getInputMouseButtonEventBlock()
		{
			return Application::get()
				->getInputMouseButtonEventBlock();
		}

		class MyTimingListener
			: public TimingListener
		{
		public:
			MyTimingListener( PyObject * _script )
				: m_script(_script)
			{
				pybind::incref(m_script);
			}

			~MyTimingListener()
			{
				pybind::decref(m_script);
			}

		protected:
			bool update( size_t _id, float _timing ) override
			{
				EventManager::get()
					->addEventFormat( EVENT_TIMING, m_script, "(if)", _id, _timing );
				
				return false;
			}

			void remove( size_t _id ) override
			{
				EventManager::get()
					->addEventFormat( EVENT_TIMING, m_script, "(iO)", _id, pybind::get_bool(true) );
			}

		protected:
			PyObject * m_script;
		};

		static size_t timing( bool _portion, bool _global, float _timing, PyObject * _script )
		{
			TimingManager * tm = Player::get()
				->getTimingManager();

			TimingListener * listener = new MyTimingListener(_script);

			size_t id = tm->timing( _portion, _global, _timing, listener );

			return id;
		}

		static void timingRemove( size_t _id )
		{
			TimingManager * tm = Player::get()
				->getTimingManager();

			tm->remove( _id );
		}

		static size_t schedule( float _timing, PyObject * _script )
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			size_t id = sm->schedule( _timing, _script );

			return id;
		}

		static void scheduleRemove( size_t _id )
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			sm->remove( _id );
		}

		static void scheduleRemoveAll()
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			sm->removeAll();
		}	

		static void s_scheduleFreeze( size_t _id, bool _freeze )
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			sm->freeze( _id, _freeze );
		}

		static void s_scheduleFreezeAll()
		{
			ScheduleManager* sm = Player::get()->getScheduleManager();
			
			sm->freezeAll( true );
		}

		static void scheduleResumeAll()
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			sm->freezeAll( false );
		}

		static bool s_scheduleIsFreeze( size_t _id )
		{
			ScheduleManager* sm = Player::get()->getScheduleManager();
			return sm->isFreeze( _id );
		}

		static float s_scheduleTime( size_t _id )
		{
			ScheduleManager* sm = Player::get()->getScheduleManager();
			float time = sm->time( _id );
			
			return time;
		}

		static size_t s_scheduleGlobal( float _timing, PyObject * _script )
		{
			ScheduleManager * sm = Player::get()->getScheduleGlobalManager();

			size_t id = sm->schedule( _timing, _script );

			return id;
		}

		static void s_scheduleGlobalRemove( size_t _id )
		{
			ScheduleManager * sm = Player::get()->getScheduleGlobalManager();

			sm->remove( _id );
		}

		static void s_scheduleGlobalRemoveAll()
		{
			ScheduleManager * sm = Player::get()->getScheduleGlobalManager();

			sm->removeAll();
		}	

		static void s_scheduleGlobalFreeze( size_t _id, bool _freeze )
		{
			ScheduleManager * sm = Player::get()->getScheduleGlobalManager();

			sm->freeze( _id, _freeze );
		}

		static void s_scheduleGlobalFreezeAll()
		{
			ScheduleManager* sm = Player::get()->getScheduleGlobalManager();

			sm->freezeAll( true );
		}

		static void s_scheduleGlobalResumeAll()
		{
			ScheduleManager * sm = Player::get()->getScheduleGlobalManager();

			sm->freezeAll( false );
		}

		static bool s_scheduleGlobalIsFreeze( size_t _id )
		{
			ScheduleManager* sm = Player::get()->getScheduleGlobalManager();

			bool freeze = sm->isFreeze( _id );

			return freeze;
		}

		static float s_scheduleGlobalTime( size_t _id )
		{
			ScheduleManager* sm = Player::get()->getScheduleGlobalManager();

			float time = sm->time( _id );

			return time;
		}

		static float getMouseX()
		{
			const Resolution& contRes = Application::get()->getContentResolution();
			float mx = Player::get()->getArrow()->getLocalPosition().x;
			mx = mt::clamp( 0.0f, mx, static_cast<float>( contRes.getWidth() ) );
			return mx;
		}

		static float getMouseY()
		{
			const Resolution& contRes = Application::get()->getContentResolution();
			float my = Player::get()->getArrow()->getLocalPosition().y;
			my = mt::clamp( 0.0f, my, static_cast<float>( contRes.getHeight() ) );
			return my;
		}

		static bool s_isMouseButtonDown( int _button )
		{
			return InputEngine::get()
				->isMouseButtonDown( _button );
		}

		static bool s_isKeyDown( int _key )
		{
			return InputEngine::get()
				->isKeyDown( static_cast<KeyCode>( _key ) );
		}

		static void s_setTimingFactor( float _factor )
		{
			 Game::get()->setTimingFactor( _factor );
		}

		static void s_addHomeless( Node * _node )
		{
			Game::get()->addHomeless( _node );
		}

		static float s_getTimingFactor( )
		{
			float factor = Game::get()->getTimingFactor();
			return factor;
		}

		static void setCurrentScene( const ConstString& _name, bool _destroyOld, bool _destroyAfterSwitch, PyObject* _cb )
		{
			MENGE_LOG_INFO( "set current scene '%s'"
				, _name.c_str()
				);

			Player::get()
				->setCurrentScene( _name, _destroyOld, _destroyAfterSwitch, _cb );
		}

		static Scene * getCurrentScene()
		{
			Scene * scene = Player::get()
				->getCurrentScene();

			return scene;
		}

		static Scene * s_createScene( const ConstString & _name )
		{
			Scene * scene = SceneManager::get()
				->createScene( _name );

			return scene;
		}

		static void s_setArrow( const ConstString & _name, const ConstString & _prototype )
		{
			Arrow * arrow = ArrowManager::get()
				->createArrow( _name, _prototype );

			if( arrow == 0 )
			{
				MENGE_LOG_ERROR( "Error: can't setup arrow '%s''%s'"
					, _name.c_str() 
					, _prototype.c_str()
					);

				return;
			}

			Player::get()->setArrow( arrow );
		}

		static Arrow * s_getArrow()
		{
			Arrow * arrow = Player::get()
				->getArrow();

			return arrow;
		}

		static const Resolution & s_getCurrentResolution()
		{
			return Application::get()
				->getCurrentResolution();
		}

		static const Resolution & s_getContentResolution()
		{
			return Application::get()
				->getContentResolution();
		}

		static void s_setArrowLayer( Layer * _layer )
		{
			Arrow * arrow = Player::get()
				->getArrow();

			_layer->addChildren( arrow );
		}

		static mt::vec2f s_getHotSpotImageSize( HotSpotImage * _hotspotImage )
		{
			if( _hotspotImage == 0 || _hotspotImage->isCompile() == false )
			{
				return mt::vec2f(0.f,0.f);
			}

			ResourceHotspotImage * resourceImage = _hotspotImage->getResourseHotspotImage();

			const mt::vec2f & size = resourceImage->getSize();

			return size;
		}

		static void setCamera2DPosition( float x, float y )
		{
			//Holder<Player>::get()
			//	->setCamera2DPosition( mt::vec2f(x, y) );
			// deprecated
		}

		static const mt::vec2f& s_getCamera2DPosition()
		{
			return Player::get()
				->getRenderCamera2D()->getViewport().begin;
		}

		static size_t s_Animatable_play( Animatable * _animatable )
		{
			float time = Player::get()
				->getTime();

			size_t id = _animatable->play( time );

			return id;
		}

		static void setCamera2DDirection( float x, float y )
		{
			assert(!"NOT IMPLEMENTED");
			//Scene * scene = getCurrentScene();
			
			//struct ForeachRender
			//	: public NodeForeach
			//{
			//	mt::vec2f dir;
			//	ForeachRender( const mt::vec2f& _dir )
			//		: dir(_dir)
			//	{}

			//	void apply( Node * children ) override
			//	{
			//		Node * child = dynamic_cast<Node*>(children);
			//		child->setLocalDirection(dir);
			//	}
			//};
			//scene->foreachChildren( ForeachRender(mt::vec2f(x,y)) );			
		}

		static void destroyNode( Node * _node )
		{
			//if( Node * parent = _node->getParent() )
			//{
			//	parent->removeChildren( _node );
			//}

			_node->destroy();
		}

		static Node * createNode( const ConstString & _name, const ConstString & _type, const ConstString & _tag )
		{
			Node * node = NodeManager::get()
				->createNode( _name, _type, _tag );

			if( node == NULL )
			{
				return NULL;
			}
			
			if( Game::get() != 0 )
			{
				Game::get()
					->addHomeless( node );
			}
			
			return node;
		}

		//static PyObject * createNodeFromBinary( const ConstString & _name, const ConstString & _tag, const ConstString & _binary )
		//{
		//	Node * node = NodeManager::get()
		//		->createNodeFromBinary( _name, _tag, _binary );

		//	if( node == 0 )
		//	{
		//		return pybind::ret_none();
		//	}

		//	Game::get()
		//		->addHomeless( node );

		//	PyObject * py_embedding = node->getEmbed();

		//	if( py_embedding == 0 )
		//	{
		//		node->destroy();
		//		return pybind::ret_none();
		//	}

		//	pybind::incref( py_embedding );

		//	return py_embedding;
		//}

		static void quitApplication()
		{
			Application::get()->quit();
		}

		static bool directResourceCompile( const ConstString& _nameResource )
		{
			bool result = ResourceManager::get()
				->directResourceCompile( _nameResource );

			return result;
		}

		static Reference *  s_getResourceReference( const ConstString& _nameResource )
		{
			Reference * reference = ResourceManager::get()->getResourceReference( _nameResource );
			if(reference == NULL )
			{
				MENGE_LOG_ERROR( "Error: (Menge.getResourceReference) not exist resource %s", _nameResource.c_str() );
				return NULL;
			}

			return reference;
		}

		static void directResourceRelease( const ConstString& _nameResource )
		{
			ResourceManager::get()
				->directResourceRelease( _nameResource );
		}

		//static void s_directResourceFileCompile( const ConstString& _category, const ConstString& _group )
		//{
		//	ResourceManager::get()
		//		->directResourceFileCompile( _category, _group );
		//}

		//static void s_deferredResourceFileCompile( PyObject* _pycategory, PyObject* _resourceFiles, PyObject* _progressCallback )
		//{
		//	if( pybind::string_check( _pycategory ) == false )
		//	{
		//		return;
		//	}

		//	ConstString category = pybind::extract<ConstString>( _pycategory );

		//	TVectorConstString resourceFiles;
		//	if( pybind::string_check( _resourceFiles ) == true )
		//	{
		//		ConstString resourceFile = pybind::extract<ConstString>( _resourceFiles );
		//		resourceFiles.push_back( resourceFile );
		//	}
		//	else if( pybind::list_check( _resourceFiles ) == true )
		//	{
		//		size_t listSize = pybind::list_size( _resourceFiles );
		//		for( size_t i = 0; i != listSize; ++i )
		//		{
		//			PyObject* listItem = pybind::list_getitem( _resourceFiles, i );
		//			if( pybind::string_check( listItem ) == false )
		//			{
		//				MENGE_LOG_ERROR( "Error: (Menge.deferredResourceFileCompile) invalid argument" );
		//				return;
		//			}
		//			ConstString resourceFile = pybind::extract<ConstString>( listItem );
		//			resourceFiles.push_back( resourceFile );
		//		}
		//	}
		//	else
		//	{
		//		MENGE_LOG_ERROR( "Error: (Menge.deferredResourceFileCompile) invalid argument" );
		//		return;
		//	}

		//	TaskDeferredLoading* task = 
		//		new TaskDeferredLoading( category, resourceFiles, _progressCallback );

		//	TaskManager::get()
		//		->addTask( task );
		//}

		//static ThreadTask * s_loadImageResources( const ConstString& _category, PyObject* _resourceFiles, PyObject* _progressCallback )
		//{
		//	TVectorConstString resourceFiles;

		//	if( pybind::string_check( _resourceFiles ) == true )
		//	{
		//		ConstString resourceFile = pybind::extract<ConstString>( _resourceFiles );
		//		resourceFiles.push_back( resourceFile );
		//	}
		//	else if( pybind::list_check( _resourceFiles ) == true )
		//	{
		//		std::size_t listSize = pybind::list_size( _resourceFiles );
		//		for( std::size_t i = 0; i != listSize; ++i )
		//		{
		//			PyObject* listItem = pybind::list_getitem( _resourceFiles, i );
		//			if( pybind::string_check( listItem ) == false )
		//			{
		//				MENGE_LOG_ERROR( "Error: (Menge.loadImageResources) invalid argument" );
		//				return NULL;
		//			}
		//			ConstString resourceFile = pybind::extract<ConstString>( listItem );
		//			resourceFiles.push_back( resourceFile );
		//		}
		//	}
		//	else
		//	{
		//		MENGE_LOG_ERROR( "Error: (Menge.loadImageResources) invalid argument" 
		//			);

		//		return NULL;
		//	}

		//	ThreadTaskLoadImageResources* task = 
		//		new ThreadTaskLoadImageResources( _category, resourceFiles, _progressCallback );

		//	ThreadTaskManager::get()
		//		->addTask( task );
		//	
		//	return task;
		//}

		//static ThreadTask * s_loadResourceImage( const ConstString& _category, const ConstString& _resourceName, PyObject* _progressCallback )
		//{
		//	ThreadTaskLoadResourceImage * task = 
		//		new ThreadTaskLoadResourceImage( _category, _resourceName, _progressCallback );

		//	ThreadTaskManager::get()
		//		->addTask( task );
		//	
		//	return task;
		//}

		static void s_cancelTask( ThreadTask * _task )
		{
			ThreadTaskManager::get()
				->cancelTask( _task );
		}

		static void s_joinTask( ThreadTask * _task )
		{
			ThreadTaskManager::get()
				->joinTask( _task );
		}

		//static void s_directResourceFileRelease( const ConstString& _category, const ConstString& _group )
		//{
		//	ResourceManager::get()
		//		->directResourceFileRelease( _category, _group );
		//}

		//static PyObject * createShot( const ConstString& _name, const mt::vec2f & _min,  const mt::vec2f & _max )
		//{
		//	mt::vec4f rect( _min, _max );

		//	ResourceImageDynamic * resourceImage 
		//		= ResourceManager::get()->getResourceT<ResourceImageDynamic>( _name );

		//	if( resourceImage == NULL )
		//	{
		//		ConstString group;

		//		Account * acc = AccountManager::get()
		//			->getCurrentAccount();

		//		if( acc != 0 )
		//		{
		//			group = acc->getFolder();
		//		}

		//		resourceImage = ResourceManager::get()
		//			->createResourceT<ResourceImageDynamic>( Consts::get()->c_user, group, _name, Consts::get()->c_ResourceImageDynamic );

		//		//FIXME
		//		TextureInterface* texture
		//			= RenderEngine::get()->createTexture( _name, 
		//			::floorf( rect[2] - rect[0] + 0.5f ), 
		//			::floorf( rect[3] - rect[1] + 0.5f ), PF_R8G8B8 );

		//		resourceImage->setRenderImage( texture );
		//	}

		//	TextureInterface* image = resourceImage->getTexture( 0 );

		//	//Holder<Application>::get()->update( 0.0f );
		//	Game::get()->tick(0.0f);

		//	RenderEngine::get()->beginScene();

		//	Game::get()->render();

		//	RenderEngine::get()->endScene();

		//	Application::get()->screenshot( image, mt::vec4f( _min.x, _min.y, _max.x, _max.y) );


		//	//image->writeToFile( "bl.bmp" );

		//	ConstString name("shotSprite");

		//	Sprite * node = NodeManager::get()
		//		->createNodeT<Sprite>( name, Consts::get()->c_Sprite, Consts::get()->c_builtin_empty );

		//	if( node == 0 )
		//	{
		//		return pybind::ret_none();
		//	}

		//	node->setImageResource( _name );

		//	node->enable();

		//	Game::get()
		//		->addHomeless( node );

		//	PyObject * py_embedding = node->getEmbed();

		//	if( py_embedding == 0 )
		//	{
		//		node->destroy();
		//		return pybind::ret_none();
		//	}

		//	pybind::incref( py_embedding );

		//	return py_embedding;		
		//}

		static void setFullscreenMode( bool _fullscreen )
		{
			Application::get()->setFullscreenMode( _fullscreen );
		}

		static bool s_getFullscreenMode()
		{
			return Application::get()->getFullscreenMode();
		}

		static void renderOneFrame()
		{
			RenderEngine::get()->beginScene();
			Game::get()->render();
			RenderEngine::get()->endScene();
			RenderEngine::get()->swapBuffers();
		}

		static void writeImageToFile( const ConstString& _resource, const WString& _filename )
		{
			ResourceImage * resource = ResourceManager::get()
				->getResourceT<ResourceImage>( _resource );

			if( resource == 0 )
			{
				MENGE_LOG_ERROR( "Error: Image resource not getting '%s'"
					, _resource.c_str() 
					);

				return;
			}

			RenderTextureInterface * img = resource->getTexture();

			RenderEngine::get()
				->saveImage( img, Consts::get()->c_user, _filename );
		}
		static void setSoundEnabled( bool _enabled )
		{
			Application::get()->setSoundEnabled( _enabled );
		}
		static void setParticlesEnabled( bool _enabled )
		{
			Application::get()->setParticlesEnabled( _enabled );
		}

		static void s_createImageResource( const ConstString& _resourceName, const ConstString& _pakName, const WString& _filename )
		{
			ResourceImageDefault* resImage = ResourceManager::get()
				->getResourceT<ResourceImageDefault>( _resourceName );

			if( resImage == NULL )
			{
				resImage = ResourceManager::get()
					->createResourceT<ResourceImageDefault>( _pakName, Consts::get()->c_builtin_empty, _resourceName, Consts::get()->c_ResourceImageDefault );
			}

			resImage->setImagePath( _filename );
		}

		//static bool createFolder( const String& _path )
		//{
		//	return Holder<FileEngine>::get()->createFolder( _path );
		//}
		//static bool deleteFolder( const String& _path )
		//{
		//	return Holder<FileEngine>::get()->deleteFolder( _path );
		//}
		static void minimizeWindow()
		{
			Application::get()->minimizeWindow();
		}

		static void s_setMouseBounded( bool _bounded )
		{
			if( Application::get()->getMouseBounded() != _bounded )
			{
				Application::get()->setMouseBounded( _bounded );
			}
		}

		static bool s_getMouseBounded()
		{
			return Application::get()->getMouseBounded();
		}

		//static bool setBlow( bool _active )
		//{
		//	return Holder<SoundServiceInterface>::get()
		//		->setBlow( _active );
		//}

		//static float getBlow()
		//{
		//	return Holder<SoundServiceInterface>::get()
		//		->getBlow();
		//}

		//static void setEnoughBlow( float _enoughBlow )
		//{
		//	Holder<SoundServiceInterface>::get()
		//		->setEnoughBlow( _enoughBlow );
		//}

		//class PySoundSulkCallback
		//	: public SoundSulkCallback
		//{
		//public:
		//	PySoundSulkCallback( PyObject * _callback )
		//		: m_callback(_callback)
		//	{
		//		pybind::incref( m_callback );
		//	}

		//	~PySoundSulkCallback()
		//	{
		//		pybind::decref( m_callback );
		//	}

		//	void blow( float _blow ) override
		//	{
		//		pybind::call( m_callback, "(f)", _blow );
		//	}

		//protected:
		//	PyObject * m_callback;
		//};

		//static void setBlowCallback( PyObject * _sulkcallback )
		//{
		//	Holder<SoundServiceInterface>::get()
		//		->setSulkCallback( new PySoundSulkCallback( _sulkcallback ) );
		//}

		static void s_enableCamera2DTargetFollowing( bool _enable, float _force )
		{
			//Player::get()
			//	->getRenderCamera2D()
			//	->enableTargetFollowing( _enable, _force );
		}

		static void s_setCamera2DBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower )
		{
			//Player::get()
			//	->getRenderCamera2D()
			//	->setBounds( _leftUpper, _rightLower );
		}

		static const mt::vec2f & s_getCursorPosition()
		{
			const mt::vec2f & pos = InputEngine::get()
				->getCursorPosition();

			return pos;
		}
		
		static bool s_isInViewport( const mt::vec2f & _pos )
		{
			return Player::get()
				->getRenderCamera2D()->getViewport().testPoint( _pos );
		}

		//static size_t s_getResourceCount( const ConstString& _category, const ConstString& _group )
		//{
		//	return ResourceManager::get()
		//		->getResourceCount( _category, _group );
		//}

		static void s_enableTextureFiltering( bool _enable )
		{
			RenderEngine::get()
				->enableTextureFiltering( _enable );
		}

		static bool s_isTextureFilteringEnabled()
		{
			return RenderEngine::get()
				->isTextureFilteringEnabled();
		}

		static bool s_existText( const ConstString & _key )
		{
			bool exist = TextManager::get()->existText( _key );

			return exist;
		}

		static PyObject * s_pickHotspot( const mt::vec2f & _point )
		{
			TVectorPickerTraps traps;

			MousePickerSystem::get()
				->pickTrap( _point, traps );

			PyObject * pyret = pybind::list_new(0);

			bool onFocus = Application::get()
				->isFocus();

			if( onFocus == false )
			{
				return pyret;
			}

			for( TVectorPickerTraps::iterator
				it = traps.begin(),
				it_end = traps.end();
			it != it_end;
			++it )
			{
				PyObject * py_embedding = (*it)->getEmbed();
				//pybind::incref( py_embedding );
				pybind::list_appenditem( pyret, py_embedding );
			}

			return pyret;
		}

		//static PhysicJoint2DInterface* s_createDistanceJoint( PhysicalBody2D* _body1, PhysicalBody2D* _body2, const mt::vec2f& _offset1, const mt::vec2f& _offset2, bool _collideBodies )
		//{
		//	return PhysicEngine2D::get()->createDistanceJoint( _body1->getInterface(), _body2->getInterface(), _offset1, _offset2, _collideBodies );
		//}

		//static PhysicJoint2DInterface* s_createHingeJoint( PhysicalBody2D* _body1, PhysicalBody2D* _body2, const mt::vec2f& _offset1, const mt::vec2f& _limits, bool _collideBodies )
		//{
		//	return PhysicEngine2D::get()->createHingeJoint( _body1->getInterface(), _body2->getInterface(), _offset1, _limits, _collideBodies );
		//}

		//static PhysicJoint2DInterface * s_createPrismaticJoint(
		//	PhysicalBody2D* _body1
		//	, PhysicalBody2D* _body2
		//	, const mt::vec2f& _unitsWorldAxis
		//	, bool _collideConnected
		//	, bool _enableLimit
		//	, const mt::vec2f& _translation
		//	, bool _enableMotor
		//	, float _maxMotorForce
		//	, float _motorSpeed)
		//{
		//	return PhysicEngine2D::get()->createPrismaticJoint( _body1->getInterface(), _body2->getInterface(), _unitsWorldAxis, _collideConnected, _enableLimit, _translation,  _enableMotor, _maxMotorForce, _motorSpeed );
		//}

		//static PhysicJoint2DInterface * s_createPulleyJoint(
		//	PhysicalBody2D* _body1
		//	, PhysicalBody2D* _body2
		//	, const mt::vec2f& _offsetBody1
		//	, const mt::vec2f& _offsetBody2
		//	, const mt::vec2f& _offsetGroundBody1
		//	, const mt::vec2f& _offsetGroundBody2
		//	, float _ratio
		//	, bool _collideConnected)
		//{
		//	return PhysicEngine2D::get()->createPulleyJoint(  _body1->getInterface(), _body2->getInterface(), _offsetBody1, _offsetBody2, _offsetGroundBody1, _offsetGroundBody2, _ratio, _collideConnected );
		//}

		//PhysicJoint2DInterface * s_createGearJoint(
		//	PhysicalBody2D * _body1
		//	, PhysicalBody2D * _body2
		//	, Menge::PhysicJoint2DInterface * _joint1
		//	, Menge::PhysicJoint2DInterface * _joint2
		//	, float _ratio
		//	, bool _collideConnected)
		//{
		//	return PhysicEngine2D::get()->createGearJoint(  _body1->getInterface(), _body2->getInterface(), _joint1, _joint2, _ratio, _collideConnected );
		//}	

		//static PhysicJoint2DInterface * s_createRopeJoint( 
		//	PhysicalBody2D * _body1
		//	, PhysicalBody2D * _body2
		//	, const mt::vec2f & _localAnchor1
		//	, const mt::vec2f & _localAnchor2
		//	, float _maxlength
		//	, bool _collideConnected )
		//{
		//	return PhysicEngine2D::get()->createRopeJoint( _body1->getInterface(), _body2->getInterface(), _localAnchor1, _localAnchor2, _maxlength ,_collideConnected );
		//}

		//static PhysicJoint2DInterface * s_createWheelJoint( 
		//	PhysicalBody2D * _body1
		//	, PhysicalBody2D * _body2
		//	, const mt::vec2f & _localAnchor1
		//	, const mt::vec2f & _localAnchor2
		//	, const mt::vec2f & _localAxis1
		//	, float _frequencyHz
		//	, float _dampingRatio
		//	, bool _collideConnected
		//	, float _maxMotorTorque
		//	, float _motorSpeed )
		//{
		//	return PhysicEngine2D::get()->createWheelJoint(  _body1->getInterface(), _body2->getInterface(), _localAnchor1, _localAnchor2, _localAxis1
		//		, _frequencyHz, _dampingRatio, _collideConnected, _maxMotorTorque, _motorSpeed );
		//}

		//static PhysicJoint2DInterface* s_createMouseJoint( PhysicalBody2D* _body, int _x, int _y )
		//{
		//	return PhysicEngine2D::get()->createMouseJoint( _body->getInterface(), _x, _y );
		//}

		//static void s_destroyJoint( PhysicJoint2DInterface* _joint )
		//{
		//	return PhysicEngine2D::get()->destroyJoint( _joint );
		//}

		//static void s_physicsMouseMove( const mt::vec2f _delta )
		//{
		//	PhysicEngine2D::get()->onMouseMove( _delta );
		//}

		static const String & s_getPlatformName()
		{
			const String & platformName = Application::get()
				->getPlatformName();

			return platformName;
		}
		//////////////////////////////////////////////////////////////////////////
		//class NodeGetChild
		//{
		//public:
		//	Node * getChild( const std::string & _name )
		//	{
		//		ConstString c_name(_name);

		//		return m_parent->findChildren( c_name, false );
		//	}

		//	void setParent( Node * _parent )
		//	{
		//		m_parent = _parent;
		//	}

		//protected:
		//	Node * m_parent;
		//};
		////////////////////////////////////////////////////////////////////////////
		//static PyObject * s_getChild( Node * _node )
		//{
		//	PyObject * py_child = pybind::list_new(0);

		//	const TListChild & child = _node->getChild();

		//	for( TListChild::const_iterator
		//		it = child.begin(),
		//		it_end = child.end();
		//	it != it_end;
		//	++it )
		//	{
		//		PyObject * py_node = it->getEmbed();
		//		pybind::list_appenditem( py_child, py_node );
		//		pybind::decref(py_node);
		//	}

		//	return py_child;
		//}
		////////////////////////////////////////////////////////////////////////////
		//static NodeGetChild * s_get_child( Node * _node )
		//{
		//	static NodeGetChild * instance = 0;

		//	if( instance == 0 )
		//	{
		//		instance = new NodeGetChild;
		//	}

		//	instance->setParent( _node );

		//	return instance;
		//}
		////////////////////////////////////////////////////////////////////////////
		//static void s_set_child( Node * _node, NodeGetChild * _obj )
		//{

		//}
		namespace SpriteAdapter
		{
			static mt::vec2f s_getLocalImageCenter( Sprite * _sprite )
			{
				const mt::vec2f & imageSize = _sprite->getImageSize();

				mt::vec2f imageCenter(imageSize.x * 0.5f, imageSize.y * 0.5f);

				return imageCenter;
			}

			static mt::vec2f s_getWorldImageCenter( Sprite * _sprite )
			{
				mt::vec2f imageCenter = s_getLocalImageCenter( _sprite );

				const mt::mat4f & wm = _sprite->getWorldMatrix();

				mt::vec2f imageCenter_wm;
				mt::mul_v2_m4( imageCenter_wm, imageCenter, wm );

				return imageCenter_wm;
			}
		}

		namespace NodeAdapter
		{
			static Node * createChildren( Node * _node, const ConstString & _name, const ConstString & _type, const ConstString & _tag )
			{
				Node * newNode = ScriptMethod::createNode(_name, _type, _tag );

				if( newNode == NULL )
				{
					return NULL;
				}

				_node->addChildren( newNode );

				return newNode;
			}
			//////////////////////////////////////////////////////////////////////////
			static void moveStop( Node * _node )
			{
				_node->stopAffectors( ETA_POSITION );
				_node->setLinearSpeed( mt::zero_v3 );
			}
			//////////////////////////////////////////////////////////////////////////
			static size_t velocityTo( Node * _node, float _speed, const mt::vec3f& _dir, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				moveStop( _node );

				Affector * affector = 
					NodeAffectorCreator::newNodeAffectorAccumulateLinear(
					_cb, ETA_POSITION, _node, &Node::setLocalPosition
					, _node->getLocalPosition(), _dir, _speed, &mt::length_v3
					);

				mt::vec3f linearSpeed = _dir * _speed;
				_node->setLinearSpeed( linearSpeed );

				size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static size_t moveTo( Node * _node, float _time, const mt::vec3f& _point, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				moveStop( _node );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateLinear(
					_cb, ETA_POSITION, _node, &Node::setLocalPosition
					, _node->getLocalPosition(), _point, _time
					, &mt::length_v3 
					);

				float invTime = 1.0f / _time;
				const mt::vec3f & pos = _node->getLocalPosition();
				mt::vec3f linearSpeed = ( _point - pos ) * invTime;

				_node->setLinearSpeed( linearSpeed );
				
				size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static size_t accMoveTo( Node * _node, float _time, const mt::vec3f& _point, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				mt::vec3f linearSpeed = _node->getLinearSpeed();

				moveStop( _node );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateQuadratic(
					_cb, ETA_POSITION, _node, &Node::setLocalPosition
					, _node->getLocalPosition(), _point, linearSpeed, _time
					, &mt::length_v3
					);
				
				size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static size_t bezier2To( Node * _node
				, float _time
				, const mt::vec3f& _point1
				, const mt::vec3f& _point2
				, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				moveStop( _node );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateQuadraticBezier(
					_cb, ETA_POSITION, _node, &Node::setLocalPosition
					, _node->getLocalPosition(), _point1, _point2, _time
					, &mt::length_v3
					);

				size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static size_t bezier3To( Node * _node
									    , float _time
									    , const mt::vec3f& _point1
									    , const mt::vec3f& _point2
									    , const mt::vec3f& _point3
									    , PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				moveStop( _node );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateCubicBezier(
					_cb, ETA_POSITION, _node, &Node::setLocalPosition
					, _node->getLocalPosition(), _point1, _point2, _point3, _time
					, &mt::length_v3
					);

				size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static void angleStop( Node * _node )
			{
				_node->stopAffectors( ETA_ANGLE );
				_node->setAngularSpeed(0.f);
			}
			//////////////////////////////////////////////////////////////////////////
			static size_t angleTo( Node * _node, float _time, float _angle, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				angleStop( _node );

				float angle = _node->getRotateZ();

				float correct_angle_from = angle;
				float correct_angle_to = _angle;
				//mt::angle_correct_interpolate_from_to( angle, _angle, correct_angle_from, correct_angle_to );

				Affector* affector =
					NodeAffectorCreator::newNodeAffectorInterpolateLinear(
					_cb, ETA_ANGLE, _node, &Node::setRotateX
					, correct_angle_from, correct_angle_to, _time
					, &fabsf 
					);				

				float invTime = 1.0f / _time;
				float angularSpeed = abs( correct_angle_from - correct_angle_to ) * invTime;

				_node->setAngularSpeed( angularSpeed );				
				size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static size_t accAngleTo( Node * _node, float _time, float _angle, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				float angularSpeed = _node->getAngularSpeed();

				angleStop( _node );

				float angle = _node->getRotateZ();

				float correct_angle_from = angle;
				float correct_angle_to = _angle;
				//mt::angle_correct_interpolate_from_to( angle, _angle, correct_angle_from, correct_angle_to );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateQuadratic(
					_cb, ETA_ANGLE, _node, &Node::setRotateX
					, correct_angle_from, correct_angle_to, angularSpeed, _time
					, &fabsf
					);				

				size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static void scaleStop( Node * _node )
			{
				_node->stopAffectors( ETA_SCALE );
			}	
			//////////////////////////////////////////////////////////////////////////
			static size_t scaleTo( Node * _node, float _time, const mt::vec3f& _scale, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				scaleStop( _node );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateLinear(
					_cb, ETA_SCALE, _node, &Node::setScale
					, _node->getScale(), _scale, _time
					, &mt::length_v3
					);

				size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static void colorStop( Node * _node )
			{
				_node->stopAffectors( ETA_COLOR );
			}
			//////////////////////////////////////////////////////////////////////////
			static size_t colorTo( Node * _node, float _time, const ColourValue& _color, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				colorStop( _node );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateLinear(
					_cb, ETA_COLOR, _node, &Colorable::setLocalColor
					, _node->getLocalColor(), _color, _time, 
					&ColourValue::length_color
					);

				size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static size_t alphaTo( Node * _node, float _time, float _alpha, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				ColourValue color = _node->getLocalColor();
				color.setA( _alpha );

				size_t id = colorTo( _node, _time, color, _cb );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static size_t setPercentVisibilityTo( Sprite * _sprite, float _time, const mt::vec4f& _percent, PyObject* _cb )
			{
				if( _sprite->isActivate() == false )
				{
					return 0;
				}

				_sprite->stopAffectors( ETA_VISIBILITY );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateLinear(
					_cb, ETA_VISIBILITY, _sprite, &Sprite::setPercentVisibility
					, _sprite->getPercentVisibility(), _percent, _time, 
					&mt::length_v4 
					);

				size_t id = _sprite->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static void setPercentVisibilityStop( Sprite * _sprite )
			{
				_sprite->stopAffectors( ETA_VISIBILITY );
			}
		};

		static PyObject * s_filterTag( Node * _node, const ConstString & _tag )
		{

			PyObject * py_filter = pybind::list_new(0);

			const TListChild & child = _node->getChild();

			for( TListChild::const_iterator
				it = child.begin(),
				it_end = child.end();
			it != it_end;
			++it )
			{
				Node * children = *it;

				const ConstString & tag = children->getTag();

				if( _tag != tag )
				{
					continue;
				}

				PyObject * py_embedding = it->getEmbed();

				pybind::list_appenditem( py_filter, py_embedding );

			}

			return py_filter;			
		}

		static Join * s_addJoin( Node * _left, Node * _right, const mt::vec2f & _offset )
		{
			Join * join = Player::get()
				->addJoin( _left, _right, _offset );

			return join;
		}

		static void s_removeJoin( Join * _join )
		{
			Player::get()
				->removeJoin( _join );
		}

		static bool s_isJoin( Node * _left, Node * _right )
		{
			bool result = Player::get()
				->isJoin( _left, _right );

			return result;
		}

		static PyObject * s_getJoins( Node * _left )
		{
			PyObject * py_list = pybind::list_new(0);

			TVectorNode joins;

			Player::get()
				->getJoins( _left, joins );

			for( TVectorNode::iterator
				it = joins.begin(),
				it_end = joins.end();
			it != it_end;
			++it )
			{
				PyObject * py_node = pybind::ptr(*it);

				pybind::list_appenditem( py_list, py_node );

				pybind::decref( py_node );
			}

			return py_list;
		}

		static Camera2D * s_getRenderCamera2D()
		{
			Camera2D * renderCamera2D = Player::get()
				->getRenderCamera2D();

			return renderCamera2D;
		}

		class AffectorScript
			: public Affector
		{
		public:
			AffectorScript( PyObject * _cb )
				: m_cb(_cb)
				, m_stop(false)
			{
				pybind::incref(m_cb);
			}

			~AffectorScript()
			{
				pybind::decref(m_cb);
			}

		public:
			bool affect( float _timing ) override
			{
				if( m_stop == true )
				{
					return true;
				}

				PyObject * py_result = pybind::ask( m_cb, "(f)", _timing );

				if( py_result == NULL )
				{
					return false;
				}

				if( pybind::bool_check(py_result) == false )
				{
					return false;
				}

				if( pybind::is_true(py_result) == true )
				{
					return true;
				}

				return false;
			}

			void stop() override
			{
				m_stop = true;
			}

		public:
			PyObject * m_cb;
			bool m_stop;		
		};

		static Affector * createAffector( PyObject * _cb )
		{
			Affector * affector = new AffectorScript( _cb );

			return affector;
		}

		static void removeAllEvent( Node * _node )
		{
			_node->removeAllEvent();
		}

		static void showKeyboard()
		{
			Application::get()
				->showKeyboard();
		}

		static void hideKeyboard()
		{
			Application::get()
				->hideKeyboard();
		}

		static bool hasResource( const ConstString & _name )
		{
			return ResourceManager::get()
				->hasResource( _name );
		}

		static void removeCurrentScene( PyObject * _cb )
		{
			Player::get()
				->removeCurrentScene( _cb );
		}

		static void setDefaultResourceFontName( const ConstString & _fontName )
		{
			TextManager::get()
				->setDefaultResourceFontName( _fontName );
		}

		static void s_addMouseMoveHandler( PyObject * _cb )
		{
			Game::get()
				->addMouseMoveHandler( _cb );
		}

		static bool s_removeMouseMoveHandler( PyObject * _cb )
		{
			bool result = Game::get()
				->removeMouseMoveHandler( _cb );

			return result;
		}

		static void s_addMouseButtonHandler( PyObject * _cb )
		{
			Game::get()
				->addMouseButtonHandler( _cb );
		}

		static bool s_removeMouseButtonHandler( PyObject * _cb )
		{
			bool result = Game::get()
				->removeMouseButtonHandler( _cb );

			return result;
		}

		static float s_getMovieDuration( const ConstString & _resourceName )
		{
			ResourceMovie * resourceMovie = ResourceManager::get()
				->getResourceT<ResourceMovie>( _resourceName );

			if( resourceMovie == NULL )
			{
				MENGE_LOG_ERROR("Mengine.getMovieDuration invalid movie resource '%s'"
					, _resourceName.c_str()
					);

				return 0.f;
			}

			float duration = resourceMovie->getDuration();

			return duration;
		}

		static bool s_addEntityPrototype( const ConstString & _prototype, PyObject * _module )
		{
			bool result = EntityManager::get()
				->addPrototype( _prototype, _module );

			return result;
		}
		
		static bool s_addScenePrototype( const ConstString & _prototype, PyObject * _module )
		{
			bool result = SceneManager::get()
				->addPrototype( _prototype, _module );

			return result;
		}

		static const Viewport & s_getLowContentViewport()
		{
			const Viewport & viewport = Application::get()
				->getLowContentViewport();

			return viewport;
		}

		static void s_destroyArrow()
		{
			Game::get()
				->destroyArrow();
		}

		static bool s_addArrowPrototype( const ConstString & _prototype, PyObject * _module )
		{
			bool result = ArrowManager::get()
				->addPrototype( _prototype, _module );

			return result;
		}

		static bool s_hasGameParam( const WString & _paramName )
		{
			if(	Game::get()
				->hasParam( _paramName ) == false )
			{
				return false;
			}

			return true;
		}

		static PyObject * s_getGameParam( const WString & _paramName )
		{
			if(	Game::get()
				->hasParam( _paramName ) == false )
			{
				return pybind::ret_none();
			}

			const WString & val = Game::get()
				->getParam( _paramName );
			
			return pybind::ptr(val);
		}
		
		static bool s_openUrlInDefaultBrowser( const WString & _url )
		{
			PlatformInterface * platform = Application::get()
				->getPlatform();
			
			bool val = platform->openUrlInDefaultBrowser( _url );

			return val;
		}

		static PyObject * s_getNullObjectsFromResourceVideo( const ConstString & _resourceName )
		{			
			class ResourceMovieVisitorNullLayers
				: public VisitorResourceMovie
			{
			public:
				ResourceMovieVisitorNullLayers( PyObject * _dictResult, float _frameDuration )
					: m_dictResult(_dictResult)
					, m_frameDuration( _frameDuration )
				{}

			protected:
				void visitLayer( const MovieLayer & _layer, const MovieLayerFrame & _frames ) override
				{
					if( _layer.source != Consts::get()->c_MovieSlot )
					{
						return;
					}
					
					PyObject * py_list_frames = pybind::list_new(0);

					if( _frames.immutable == false )
					{
						for( TVectorMovieFrameSource::size_type
							it_frame = 0,
							it_frame_end = _frames.frames.size();
						it_frame != it_frame_end;
						++it_frame )
						{
							const MovieFrameSource & frame_source = _frames.frames[it_frame];

							PyObject * py_dict_frame = pybind::dict_new();

							PyObject * py_pos = pybind::ptr( frame_source.position );
							pybind::dict_set( py_dict_frame, "position", py_pos );
							pybind::decref( py_pos );

							float frameTime = _layer.in + it_frame * m_frameDuration;

							PyObject * py_time = pybind::ptr( frameTime );
							pybind::dict_set( py_dict_frame, "time", py_time );
							pybind::decref( py_time );

							pybind::list_appenditem( py_list_frames, py_dict_frame );
							pybind::decref( py_dict_frame );
						}
					}
					else
					{
						for( size_t
							it_frame = 0,
							it_frame_end = _frames.count;
						it_frame != it_frame_end;
						++it_frame )
						{
							PyObject * py_dict_frame = pybind::dict_new();

							PyObject * py_pos = pybind::ptr( _frames.source.position );
							pybind::dict_set( py_dict_frame, "position", py_pos );
							pybind::decref( py_pos );

							float frameTime = _layer.in + it_frame * m_frameDuration;

							PyObject * py_time = pybind::ptr( frameTime );
							pybind::dict_set( py_dict_frame, "time", py_time );
							pybind::decref( py_time );

							pybind::list_appenditem( py_list_frames, py_dict_frame );
							pybind::decref( py_dict_frame );
						}
					}

					pybind::dict_set( m_dictResult, _layer.name.c_str(), py_list_frames );
					pybind::decref( py_list_frames );
				}

			protected:
				PyObject * m_dictResult;
				float m_frameDuration;
			};
			
			////////////////////////////////////////////////////////////////////////

			ResourceMovie* resource = ResourceManager::get()
				->getResourceT<ResourceMovie>( _resourceName );
											
			if( resource == NULL )
			{
				return pybind::ret_none();
			}

			PyObject * py_dict_result = pybind::dict_new();

			float frameTime = resource->getFrameDuration();
			ResourceMovieVisitorNullLayers visitor( py_dict_result, frameTime );
			
			resource->visitResourceMovie( &visitor );

			return py_dict_result;
		}


		static PyObject * s_visitResourceEmitterContainer( const ConstString & _resourceName )
		{
			class ResourceEmitterContainerVisitor:
				public EmitterContainerInterface::EmitterContainerVisitor
			{
			public:
				void visitEmitterName( const String& _emitterName )
				{
					catchedNames.push_back( _emitterName );
				}

				void visitAtlas( const EmitterContainerInterface::EmitterAtlas & _atlas )
				{
					cathedAtlas.push_back( _atlas );
				}

				EmitterContainerInterface::TVectorAtlas cathedAtlas;
				TVectorString catchedNames;
			};
		
			ResourceEmitterContainer* resource = ResourceManager::get()
				->getResourceT<ResourceEmitterContainer>( _resourceName );

			if( resource == NULL )
			{
				return pybind::ret_none();
			}

			PyObject * py_dict_result = pybind::dict_new();
			
			PyObject * py_list_names = pybind::list_new(0);
			PyObject * py_list_atlas = pybind::list_new(0);

			ResourceEmitterContainerVisitor visitor;
			EmitterContainerInterface * container = resource->getContainer();
			container->visitContainer( &visitor );

			for( TVectorString::const_iterator
				it = visitor.catchedNames.begin(),
				it_end = visitor.catchedNames.end();
			it != it_end;
			++it )
			{
				PyObject * py_node = pybind::ptr(*it);
				pybind::list_appenditem( py_list_names, py_node );
				pybind::decref( py_node );
			}

			for( EmitterContainerInterface::TVectorAtlas::const_iterator
				it = visitor.cathedAtlas.begin(),
				it_end = visitor.cathedAtlas.end();
			it != it_end;
			++it )
			{
				PyObject * py_dict = pybind::dict_new();
				PyObject * py_node_file = pybind::ptr( (*it).file );
				//PyObject * py_node_path = pybind::ptr((*it).path);
				
				pybind::dict_set( py_dict, "file", py_node_file );
				//pybind::dict_set(py_dict,"path",py_node_path);
				
				pybind::decref( py_node_file );
				//pybind::decref( py_node_path );
				pybind::list_appenditem( py_list_atlas, py_dict );
			}

			pybind::dict_set( py_dict_result, "Emitters", py_list_names );
			pybind::dict_set( py_dict_result, "Atlasses", py_list_atlas );

			return py_dict_result;
		}
		//////////////////////////////////////////////////////////////////////////		
		class ResourceVisitorGetAlreadyCompiled
			: public ResourceVisitor
		{
		public:
			ResourceVisitorGetAlreadyCompiled( PyObject * _cb )
				: m_cb(_cb)
			{
				pybind::incref( m_cb );
			}

			~ResourceVisitorGetAlreadyCompiled()
			{
				pybind::decref( m_cb );
			}

		protected:
			void visit( ResourceReference* _resource )
			{
				if( _resource->isCompile() == false )
				{
					return;
				}

				PyObject * py_resource = pybind::ptr( _resource );

				pybind::call( m_cb, "(O)", py_resource );

				pybind::decref( py_resource );
			}

		protected:
			PyObject * m_cb;
		};

		static void s_visitChild( Node * _node, PyObject * _cb )
		{
			const TListChild & child = _node->getChild();
			
			for( TListChild::const_iterator
				it = child.begin(),
				it_end = child.end();
			it != it_end;
			++it )
			{
				Node * children = *it;
				PyObject * py_children = pybind::ptr( children);

				pybind::call( _cb, "(O)", py_children );

				pybind::decref( py_children );
			}
		}


		static void s_visitCompiledResources( const ConstString & _category, const ConstString & _groupName, PyObject * _cb )
		{
			ResourceVisitorGetAlreadyCompiled rv_gac(_cb);

			ResourceManager::get()
				->visitResources( _category, _groupName, &rv_gac );						
		}
		
		static bool s_validResource( const ConstString & _resourceName )
		{
			bool valid = ResourceManager::get()
				->validResource( _resourceName );

			return valid;
		}
	}

	static void classWrapping()
	{
		SCRIPT_CLASS_WRAPPING( Node );
		SCRIPT_CLASS_WRAPPING( Layer );
		SCRIPT_CLASS_WRAPPING( Layer2D );
		SCRIPT_CLASS_WRAPPING( Layer2DPhysic );
		SCRIPT_CLASS_WRAPPING( HotSpot );
		//SCRIPT_CLASS_WRAPPING( Light2D );
		//SCRIPT_CLASS_WRAPPING( ShadowCaster2D );
		SCRIPT_CLASS_WRAPPING( Sprite );
		SCRIPT_CLASS_WRAPPING( Animation );
		SCRIPT_CLASS_WRAPPING( Arrow );
		SCRIPT_CLASS_WRAPPING( TextField );
		SCRIPT_CLASS_WRAPPING( SoundEmitter );
		SCRIPT_CLASS_WRAPPING( ParticleEmitter );
		SCRIPT_CLASS_WRAPPING( Movie );
		SCRIPT_CLASS_WRAPPING( MovieInternalObject );
		SCRIPT_CLASS_WRAPPING( Point );
		//SCRIPT_CLASS_WRAPPING( TilePolygon );
		SCRIPT_CLASS_WRAPPING( Video );
		//SCRIPT_CLASS_WRAPPING( FFCamera3D );
		//SCRIPT_CLASS_WRAPPING( DiscreteEntity );
		//SCRIPT_CLASS_WRAPPING( RigidBody3D );
		//SCRIPT_CLASS_WRAPPING( Layer3D );
		//SCRIPT_CLASS_WRAPPING( RigidBody2D );
		//SCRIPT_CLASS_WRAPPING( PhysicalBody2D );
		//SCRIPT_CLASS_WRAPPING( CapsuleController );
		//SCRIPT_CLASS_WRAPPING( SceneNode3D );
		//SCRIPT_CLASS_WRAPPING( Camera3D );
		//SCRIPT_CLASS_WRAPPING( RenderMesh );
		SCRIPT_CLASS_WRAPPING( Window );
		
		SCRIPT_CLASS_WRAPPING( HotSpotImage );
		SCRIPT_CLASS_WRAPPING( Camera2D );
		//SCRIPT_CLASS_WRAPPING( Layer2DTexture );
	}

	struct FPyStringLess
		: public std::binary_function<PyObject *, PyObject *, bool>
	{
		bool operator()(PyObject* _Left, PyObject* _Right) const
		{	// apply operator< to operands
			const char * left_char = pybind::string_to_char(_Left);
			const char * right_char = pybind::string_to_char(_Right);

			int cmp = strcmp( left_char, right_char );

			return cmp < 0;
		}
	};
	
	struct extract_ConstString_type
		: public pybind::type_cast_result<ConstString>
	{
	public:
		bool apply( PyObject * _obj, ConstString & _value ) override
		{
            const char * value_char = pybind::string_to_char( _obj );
            
            _value = ConstString(value_char);

			return true;
		}
        
		PyObject * wrap( pybind::type_cast_result<ConstString>::TCastRef _value ) override
		{
			PyObject * py_value = pybind::string_from_char_size( _value.c_str(), _value.size() );

			return py_value;
			//return pybind::string_from_char( _value.c_str(), _value.size() );
		}
    };

	struct extract_TVectorString_type
		: public pybind::type_cast_result<TVectorString>
	{
		bool apply( PyObject * _obj, TVectorString & _value ) override
		{
			TVectorString value;

			if( pybind::list_check( _obj ) == true )
			{
				size_t size = pybind::list_size( _obj );

				for( size_t it = 0; it != size; ++it )
				{
					PyObject * py_string = pybind::list_getitem( _obj, it );

					String key;
					pybind::extract_value( py_string, key );

					_value.push_back( key );

					pybind::decref( py_string );
				}
			}
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( pybind::type_cast_result<TVectorString>::TCastRef _value ) override
		{
			PyObject * py_value = pybind::list_new(0);

			for( TVectorString::const_iterator
				it = _value.begin(),
				it_end = _value.end();
			it != it_end;
			++it )
			{
				PyObject * py_string = pybind::ptr( *it );

				pybind::list_appenditem( py_value, py_string );

				pybind::decref( py_string );
			}

			return py_value;
		}
	};

	struct extract_TVectorWString_type
		: public pybind::type_cast_result<TVectorWString>
	{
		bool apply( PyObject * _obj, TVectorWString & _value ) override
		{
			TVectorWString value;

			if( pybind::list_check( _obj ) == true )
			{
				size_t size = pybind::list_size( _obj );

				for( size_t it = 0; it != size; ++it )
				{
					PyObject * py_string = pybind::list_getitem( _obj, it );

					WString key;
					pybind::extract_value( py_string, key );

					_value.push_back( key );

					pybind::decref( py_string );
				}
			}
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( pybind::type_cast_result<TVectorWString>::TCastRef _value ) override
		{
			PyObject * py_value = pybind::list_new(0);

			for( TVectorWString::const_iterator
				it = _value.begin(),
				it_end = _value.end();
			it != it_end;
			++it )
			{
				PyObject * py_string = pybind::ptr( *it );

				pybind::list_appenditem( py_value, py_string );

				pybind::decref( py_string );
			}

			return py_value;
		}
	};

	//REGISTER_SCRIPT_CLASS( Menge, Node, Base )

	void ScriptWrapper::nodeWrap()
	{
		pybind::registration_type_cast<ConstString>( new extract_ConstString_type ); 
		pybind::registration_type_cast<TVectorString>( new extract_TVectorString_type );
		pybind::registration_type_cast<TVectorWString>( new extract_TVectorWString_type );

		classWrapping();
		
		pybind::interface_<Join>("Join")
			.def("getLeft", &Join::getLeft)
			.def("getRight", &Join::getRight)
			.def("getOffset", &Join::getOffset)
			;

		pybind::interface_<Affector>("Affector", true)
			.def( "stop", &Affector::stop )
			;

		pybind::interface_<Scriptable>("Scriptable")
			;

		pybind::interface_<Identity>("Identity")
			.def( "setName", &Identity::setName )
			.def( "getName", &Identity::getName )
			.def( "getType", &Identity::getType )
			.def( "setTag", &Identity::setTag )
			.def( "getTag", &Identity::getTag )
			.def( "getUniqueId", &Identity::getUniqueId )
			;

		pybind::interface_<Transformation3D>("Transformation3D")
			.def( "setLocalPosition", &Transformation3D::setLocalPosition )
			.def( "getLocalPosition", &Transformation3D::getLocalPosition )
			//.def( "getLocalDirection", &Transformation3D::getLocalDirection )
			//.def( "setLocalDirection", &Transformation3D::setLocalDirection )
			.def( "setCoordinate", &Transformation3D::setCoordinate )			
			.def( "getCoordinate", &Transformation3D::getCoordinate )
			.def( "setOrigin", &Transformation3D::setOrigin )
			.def( "getOrigin", &Transformation3D::getOrigin )
			.def( "setScale", &Transformation3D::setScale )
			.def( "getScale", &Transformation3D::getScale )
			.def( "setRotateX", &Transformation3D::setRotateX )
			.def( "getRotateX", &Transformation3D::getRotateX )
			.def( "setRotateY", &Transformation3D::setRotateY )
			.def( "getRotateY", &Transformation3D::getRotateY )
			.def( "setRotateZ", &Transformation3D::setRotateZ )
			.def( "getRotateZ", &Transformation3D::getRotateZ )
			.def( "setAngle", &Transformation3D::setRotateX )
			.def( "getAngle", &Transformation3D::getRotateX )

			.def( "translate", &Transformation3D::translate )

			.def( "resetTransformation", &Transformation3D::resetTransformation )

			//.def( "setRotate", &Transformation3D::setAngle ) //depricated
			;

		//pybind::class_<FFCamera3D>("FFCamera3D")
		//	.def( pybind::init<>() )
		//	.def( "update", &FFCamera3D::update )
		//	.def( "activate", &FFCamera3D::activate )
		//	.def( "forward", &FFCamera3D::forward )
		//	.def( "left", &FFCamera3D::left )
		//	.def( "setActor", &FFCamera3D::setActor )
		//	.def( "yaw", &FFCamera3D::yaw )
		//	.def( "pitch", &FFCamera3D::pitch )
		//	.def( "zoom", &FFCamera3D::zoom )
		//	;

		//pybind::interface_<NodeRenderable>("NodeRenderable", false)
		//	.def( "hide", &NodeRenderable::hide )
		//	;

		pybind::interface_<Resource>("Resource")
			.def( "compile", &Resource::compile )
			.def( "release", &Resource::release )
			.def( "isCompile", &Resource::isCompile )
			;

		pybind::interface_<Reference>("Reference")
			.def( "incrementReference", &Reference::incrementReference )
			.def( "decrementReference", &Reference::decrementReference )
			.def( "countReference", &Reference::countReference )
			;

		pybind::interface_<ResourceReference, pybind::bases<Resource, Identity, Reference> >("ResourceReference")
			.def( "getCategory", &ResourceReference::getCategory )
			.def( "getGroup", &ResourceReference::getGroup )
			;

		pybind::interface_<Renderable>("Renderable")
			.def( "hide", &Renderable::hide )
			.def( "isHide", &Renderable::isHide )
			.def( "localHide", &Renderable::localHide )
			.def( "isLocalHide", &Renderable::isLocalHide )
			;

		pybind::interface_<Colorable>("Colorable")
			.def( "setLocalColor", &Colorable::setLocalColor )
			.def( "getLocalColor", &Colorable::getLocalColor )
			.def( "setLocalAlpha", &Colorable::setLocalAlpha )
			.def( "getLocalAlpha", &Colorable::getLocalAlpha )
			.def( "setPersonalColor", &Colorable::setPersonalColor )
			.def( "getPersonalColor", &Colorable::getPersonalColor )
			.def( "setPersonalAlpha", &Colorable::setPersonalAlpha )
			.def( "getPersonalAlpha", &Colorable::getPersonalAlpha )			
			;

		pybind::interface_<Animatable>("Animatable")
			.def_static( "play", &ScriptMethod::s_Animatable_play )
			.def( "stop", &Animatable::stop )
			.def( "isPlay", &Animatable::isPlay )
			.def( "getPlayId", &Animatable::getPlayId )
			.def( "setSpeedFactor", &Animatable::setSpeedFactor )
			.def( "getSpeedFactor", &Animatable::getSpeedFactor )
			.def( "setFirstFrame", &Animatable::setFirstFrame )
			.def( "setLastFrame", &Animatable::setLastFrame )
			.def( "setTiming", &Animatable::setTiming )
			.def( "getTiming", &Animatable::getTiming )
			.def( "setLoop", &Animatable::setLoop )
			.def( "getLoop", &Animatable::getLoop )
			.def( "interrupt", &ParticleEmitter::interrupt )
			;

		pybind::interface_<GlobalHandleAdapter>("GlobalHandleAdapter")
			.def( "enableGlobalMouseEvent", &GlobalHandleAdapter::enableGlobalMouseEvent )
			.def( "enableGlobalKeyEvent", &GlobalHandleAdapter::enableGlobalKeyEvent )				
			;

		pybind::interface_<Affectorable>("Affectorable")
			.def( "addAffector", &Affectorable::addAffector )
			;

		pybind::interface_<Node, pybind::bases<Scriptable, Identity, Transformation3D, Colorable, Resource, Renderable, GlobalHandleAdapter, Affectorable> >("Node", false)
			.def( "enable", &Node::enable )
			.def( "disable", &Node::disable )
			.def( "isEnable", &Node::isEnable )
			.def( "render", &Node::render )
			.def( "isActivate", &Node::isActivate )
			.def( "freeze", &Node::freeze )
			.def( "isFreeze", &Node::isFreeze )
			.def( "addChildren", &Node::addChildren )
			.def( "addChildrenFront", &Node::addChildrenFront )
			.def( "addChildrenAfter", &Node::addChildrenAfter )
			.def( "removeChildren", &Node::removeChildren )
			.def( "removeAllChild", &Node::removeAllChild )
			.def( "removeFromParent", &Node::removeFromParent )
			//.def_static( "getChild", &ScriptMethod::s_getChild )
			.def( "findChildren", &Node::findChildren )
			.def( "findTag", &Node::findTag )
			.def_static( "filterTag", &ScriptMethod::s_filterTag )
			.def( "hasChildren", &Node::hasChildren )
			.def( "emptyChild", &Node::emptyChild )
			.def( "update", &Node::update )
			.def( "getParent", &Node::getParent )
			.def( "getScene", &Node::getScene )
			.def( "getLayer", &Node::getLayer )
			.def_native( "setEventListener", &Node::setEventListener )
			.def( "removeEventListener", &Node::removeEventListener )
			//.def( "getListener", &Node::getListener )

			.def( "getWorldPosition", &Node::getWorldPosition )
			//.def( "getWorldDirection", &Node::getWorldDirection )
			.def( "getCameraPosition", &Node::getCameraPosition )

			.def( "setWorldPosition", &Node::setWorldPosition )

			.def( "getWorldColor", &Node::getWorldColor )

			.def_static( "createChildren", &ScriptMethod::NodeAdapter::createChildren )

			.def_static( "colorTo", &ScriptMethod::NodeAdapter::colorTo )
			.def_static( "alphaTo", &ScriptMethod::NodeAdapter::alphaTo )
			.def_static( "colorStop", &ScriptMethod::NodeAdapter::colorStop )

			.def_static( "velocityTo", &ScriptMethod::NodeAdapter::velocityTo )

			.def_static( "moveTo", &ScriptMethod::NodeAdapter::moveTo )
			.def_static( "bezier2To", &ScriptMethod::NodeAdapter::bezier2To )
			.def_static( "bezier3To", &ScriptMethod::NodeAdapter::bezier3To )
			.def_static( "moveStop", &ScriptMethod::NodeAdapter::moveStop )

			.def_static( "angleTo", &ScriptMethod::NodeAdapter::angleTo )
			.def_static( "angleStop", &ScriptMethod::NodeAdapter::angleStop )
			.def_static( "scaleTo", &ScriptMethod::NodeAdapter::scaleTo )
			.def_static( "scaleStop", &ScriptMethod::NodeAdapter::scaleStop )

			.def_static( "accMoveTo", &ScriptMethod::NodeAdapter::accMoveTo )
			.def_static( "accAngleTo", &ScriptMethod::NodeAdapter::accAngleTo )

			//.def_property_static( "child", &ScriptMethod::s_get_child, &ScriptMethod::s_set_child )
			;

		pybind::interface_<ThreadTask>("Task")
			;

		pybind::interface_<RenderCameraInterface>("RenderCameraInterface")
			.def( "getViewport", &RenderCameraInterface::getViewport )
			;
		
		pybind::proxy_<Camera2D, pybind::bases<Node, RenderCameraInterface> >("Camera2D", false)
			.def( "setViewport", &Camera2D::setViewport )			
			//.def( "setRenderport", &Camera2D::setRenderport )
			//.def( "getRenderport", &Camera2D::getRenderport )
			.def( "setRenderTarget", &Camera2D::setRenderTarget )
			;		

		//pybind::proxy_<SceneNode3D, pybind::bases<Node>>("SceneNode3D", false)
		//	.def( "getWorldOrient", &SceneNode3D::getWorldOrient )
		//	.def( "getWorldPosition", &SceneNode3D::getWorldPosition )
		//	.def( "getLocalOrient", &SceneNode3D::getLocalOrient )
		//	.def( "getPosition", &SceneNode3D::getLocalPosition )
		//	.def( "setPosition", &SceneNode3D::setLocalPosition )
		//	.def( "setOrient", &SceneNode3D::setLocalOrient )
		//	.def( "setScale", &SceneNode3D::setScale )
		//	.def( "yaw", &SceneNode3D::yaw )
		//	.def( "pitch", &SceneNode3D::pitch )
		//	.def( "roll", &SceneNode3D::roll )
		//	.def( "setFixedYawAxis", &SceneNode3D::setFixedYawAxis )
		//	.def( "translate", &SceneNode3D::translate )
		//	.def( "addChild", &SceneNode3D::addChild )
		//	.def( "setYawSpeed", &SceneNode3D::setYawSpeed )
		//	.def( "setYawLimits", &SceneNode3D::setYawLimits )
		//	.def( "getYaw", &SceneNode3D::getYaw )
		//	.def( "getPitch", &SceneNode3D::getPitch )
		//	.def( "setListener", &SceneNode3D::setListener )
		
		//	//.def( "getCamera", &SceneNode3D::getCamera )
		//	;

		{

			//pybind::proxy_<RigidBody3D, pybind::bases<Node>>("RigidBody3D", false)
			//	.def( "applyForce", &RigidBody3D::applyForce )
			//	.def( "applyImpulse", &RigidBody3D::applyImpulse )
			//	.def( "applyAngularImpulse", &RigidBody3D::applyAngularImpulse )
			//	.def( "applyTorque", &RigidBody3D::applyTorque )
			//	.def( "setLinearVelocity", &RigidBody3D::setLinearVelocity )
			//	.def( "setAngularVelocity", &RigidBody3D::setAngularVelocity )
			//	.def( "setActive", &RigidBody3D::setActive )
			//	;

			//pybind::proxy_<CapsuleController, pybind::bases<Node>>("CapsuleController", false)
			//	.def( "move", &CapsuleController::move )
			//	.def( "setPosition", &CapsuleController::setPosition )
			//	.def( "getFilteredPosition", &CapsuleController::getFilteredPosition )
			//	;

			//pybind::proxy_<Camera3D, pybind::bases<SceneNode3D> >("Camera3D", false)
			//	.def( "setPosition", &Camera3D::setPosition )
			//	.def( "lookAt", &Camera3D::lookAt )
			//	//	.def( "yaw", &Camera3D::yaw )
			//	//	.def( "pitch", &Camera3D::pitch )
			//	//	.def( "roll", &Camera3D::roll )
			//	.def( "getDirection", &Camera3D::getDirection )
			//	;

			//pybind::proxy_<DiscreteEntity, pybind::bases<SceneNode3D>>("DiscreteEntity", false)
			//	.def( "createRenderToTexture", &DiscreteEntity::createRenderToTexture )
			//	.def( "playAnimation", &DiscreteEntity::playAnimation )
			//	.def( "setMaterial", &DiscreteEntity::setMaterial )
			//	//.def( "playAnimation", &DiscreteEntity::playAnimation )
			//	;

			//pybind::interface_<Allocator3D>("Allocator3D", false)
			//	.def( "scale", &Allocator3D::scale )
			//	;

			//pybind::proxy_<SceneNode3D, pybind::bases<Node, Allocator3D> >("SceneNode3D", false)
			//	.def( "yaw", &SceneNode3D::yaw )
			//	.def( "pitch", &SceneNode3D::pitch )
			//	.def( "getYaw", &SceneNode3D::getYaw )
			//	.def( "getPitch", &SceneNode3D::getPitch )
			//	.def( "yawTime", &SceneNode3D::yawTime )
			//	.def( "pitchTime", &SceneNode3D::pitchTime )
			//	.def( "setListener", &SceneNode3D::setListener )
			//	;

			//pybind::proxy_<Layer3D, pybind::bases<SceneNode3D> >("Layer3D", false)
			//	//.def( "addCamera", &Layer3D::addCamera )
			//	//.def( "getCamera", &Layer3D::getCamera )	
			//	.def( "addController", &Layer3D::addController )
			//	.def( "getController", &Layer3D::getController )		
			//	//.def( "getNode", &Layer3D::getNode )
			//	;

			{

				//pybind::proxy_< RenderMesh, pybind::bases<SceneNode3D> >("RenderMesh", false)
				//	//.def( "createRenderTarget", &RenderMesh::createRenderTarget )
				//	//.def( "setMaterial", &RenderMesh::setMaterial )
				//	;

				//pybind::proxy_<Camera3D, pybind::bases<SceneNode3D> >("Camera3D", false)
				//	//	.def( "setPosition", &Camera3D::setPosition )
				//	.def( "lookAt", &Camera3D::lookAt )
				//	.def( "yaw", &Camera3D::yaw )
				//	.def( "pitch", &Camera3D::pitch )
				//	//.def( "roll", &Camera3D::roll )
				//	//	.def( "getDirection", &Camera3D::getDirection )
				//	;
			}

			{
				pybind::proxy_<ParticleEmitter, pybind::bases<Node, Animatable> >("ParticleEmitter", false)
					.def( "playFromPosition", &ParticleEmitter::playFromPosition )
					.def( "pause", &ParticleEmitter::pause )
					.def( "restart", &ParticleEmitter::restart )
					.def( "setLoop", &ParticleEmitter::setLoop )
					.def( "getLoop", &ParticleEmitter::getLoop )
					//.def( "interrupt", &ParticleEmitter::interrupt )
					.def( "setLeftBorder", &ParticleEmitter::setLeftBorder )
					.def( "setResource", &ParticleEmitter::setResource )
					.def( "setEmitter", &ParticleEmitter::setEmitter )
					.def( "setEmitterTranslateWithParticle", &ParticleEmitter::setEmitterTranslateWithParticle )
					.def( "setEmitterRelative", &ParticleEmitter::setEmitterRelative )
					.def( "setEmitterImage", &ParticleEmitter::setEmitterImage )
					.def( "changeEmitterPolygon", &ParticleEmitter::changeEmitterPolygon )
					.def( "setStartPosition", &ParticleEmitter::setStartPosition )
					.def( "getEmitterName" , &ParticleEmitter::getEmitterName )
					.def( "getLeftBorder" , &ParticleEmitter::getLeftBorder )
					.def( "getRightBorder" , &ParticleEmitter::getRightBorder )
					.def( "getDuration" , &ParticleEmitter::getDuration )
					.def( "setPositionOffset" , &ParticleEmitter::setPositionOffset )
					.def( "getEmitterBoundingBox", &ParticleEmitter::getEmitterBoundingBox )
					.def( "getEmitterPosition", &ParticleEmitter::getEmitterPosition )
					.def( "getRelativeSize", &ParticleEmitter::getRelativeSize )
					.def( "setRandomMode", &ParticleEmitter::setRandomMode )
					.def( "getRandomMode", &ParticleEmitter::getRandomMode )
					;

				pybind::proxy_<SoundEmitter, pybind::bases<Node, Animatable> >("SoundEmitter", false)
					.def( "setVolume", &SoundEmitter::setVolume )
					.def( "getVolume", &SoundEmitter::getVolume )
					.def( "setLoop", &SoundEmitter::setLoop )
					.def( "isLooping", &SoundEmitter::getLoop )
					.def( "setSoundResource", &SoundEmitter::setSoundResource )
					;

				pybind::proxy_<TextField, pybind::bases<Node> >("TextField", false)
					.def( "setText", &TextField::setText )
					.def( "getText", &TextField::getText )
					.def( "setHeight", &TextField::setHeight )
					.def( "getHeight", &TextField::getHeight )
					.def( "setOutlineColor", &TextField::setOutlineColor )
					.def( "getOutlineColor", &TextField::getOutlineColor )
					.def( "enableOutline", &TextField::enableOutline )
					.def( "isOutline", &TextField::isOutline )
					.def( "getLength", &TextField::getLength )
					.def( "setMaxLen", &TextField::setMaxLen )
					.def( "getLineOffset", &TextField::getLineOffset )
					.def( "setLineOffset", &TextField::setLineOffset )
					.def( "setResourceFont", &TextField::setResourceFont )
					.def( "getResourceFont", &TextField::getResourceFont )

					.def( "setNoneAlign", &TextField::setNoneAlign )
					.def( "isNoneAlign", &TextField::isNoneAlign )
					.def( "setCenterAlign", &TextField::setCenterAlign )
					.def( "isCenterAlign", &TextField::isCenterAlign )
					.def( "setRightAlign", &TextField::setRightAlign )
					.def( "isRightAlign", &TextField::isRightAlign )
					.def( "setLeftAlign", &TextField::setLeftAlign )
					.def( "isLeftAlign", &TextField::isLeftAlign )

					.def( "setVerticalNoneAlign", &TextField::setVerticalNoneAlign )
					.def( "isVerticalNoneAlign", &TextField::isVerticalNoneAlign )
					.def( "setVerticalCenterAlign", &TextField::setVerticalCenterAlign )
					.def( "isVerticalCenterAlign", &TextField::isVerticalCenterAlign )
					
					.def( "getCharOffset", &TextField::getCharOffset )
					.def( "setCharOffset", &TextField::setCharOffset )
					.def( "setTextByKey", &TextField::setTextByKey )
					.def( "setTextByKeyFormat", &TextField::setTextByKeyFormat )					
					.def( "getTextKey", &TextField::getTextKey )

					.def( "setPixelsnap", &TextField::setPixelsnap )
					.def( "getPixelsnap", &TextField::getPixelsnap )
					
					.def( "setMaxCharCount", &TextField::setMaxCharCount )
					.def( "getMaxCharCount", &TextField::getMaxCharCount )
					.def( "getCharCount", &TextField::getCharCount )
					;
				
				pybind::proxy_<Point, pybind::bases<Node> >("Point", false)
					.def( "testHotSpot", &Point::testHotSpot )
					;

				pybind::proxy_<Layer, pybind::bases<Node> >("Layer", false)
					.def( "setMain", &Layer::setMain )
					.def( "isMain", &Layer::isMain )
					.def( "setSize", &Layer::setSize )
					.def( "getSize", &Layer::getSize )
					;

				pybind::proxy_<Layer2D, pybind::bases<Layer> >("Layer2D", false)
					.def( "setParallaxFactor", &Layer2D::setParallaxFactor )
					.def( "getParallaxFactor", &Layer2D::getParallaxFactor )
					.def( "setRenderViewport", &Layer2D::setRenderViewport )
					.def( "removeRenderViewport", &Layer2D::removeRenderViewport )
					.def( "cameraToLocal", &Layer2D::cameraToLocal )
					;
				pybind::proxy_<Layer2DPhysic, pybind::bases<Layer2D> >("Layer2DPhysic", false);
				//pybind::proxy_<Layer2DTexture, pybind::bases<Layer2D> >("Layer2DTexture", false)
				//	.def( "setCameraOffset", &Layer2DTexture::setCameraOffset )
				//	.def( "getCameraOffset", &Layer2DTexture::getCameraOffset )
				//	.def( "setRenderTargetName", &Layer2DTexture::setRenderTargetName )
				//	.def( "getRenderTargetName", &Layer2DTexture::getRenderTargetName )
				//	;

				pybind::interface_<MousePickerTrap>("MousePickerTrap", false)
					.def( "pick", &MousePickerTrap::pick )
					;

				pybind::interface_<MousePickerAdapter, pybind::bases<MousePickerTrap> >("MousePickerAdapter", false)
					;

				pybind::proxy_<HotSpot, pybind::bases<Node, MousePickerAdapter> >("HotSpot", false)
					.def( "testPoint", &HotSpot::testPoint )
					.def( "clearPoints", &HotSpot::clearPoints )
					.def( "getPolygon", &HotSpot::getPolygon )
					.def( "setPolygon", &HotSpot::setPolygon )
					.def_static( "getLocalPolygonCenter", &ScriptMethod::HotSpotAdapter::s_getLocalPolygonCenter )
					.def_static( "getWorldPolygonCenter", &ScriptMethod::HotSpotAdapter::s_getWorldPolygonCenter )
					.def_static( "getWorldPolygon", &ScriptMethod::HotSpotAdapter::s_getWorldPolygon )
					;

				pybind::proxy_<HotSpotImage, pybind::bases<HotSpot> >("HotSpotImage", false)
					.def( "setImageResource", &HotSpotImage::setImageResource )
					.def( "getImageResource", &HotSpotImage::getImageResource )
					//.def( "setFrame", &HotSpotImage::setFrame )
					//.def( "getFrame", &HotSpotImage::getFrame )
					.def( "setAlphaTest", &HotSpotImage::setAlphaTest )
					.def( "getAlphaTest", &HotSpotImage::getAlphaTest )
					;
				
				pybind::proxy_<Sprite, pybind::bases<Node> >("Sprite", false)
					//.def( "setImageIndex", &Sprite::setImageIndex )
					//.def( "getImageIndex", &Sprite::getImageIndex )
					//.def( "getImageCount", &Sprite::getImageCount )
					.def( "setImageResource", &Sprite::setImageResource )
					.def( "getImageResource", &Sprite::getImageResource )
					.def( "getImageSize", &Sprite::getImageSize )
					.def_static( "getLocalImageCenter", &ScriptMethod::SpriteAdapter::s_getLocalImageCenter )
					.def_static( "getWorldImageCenter", &ScriptMethod::SpriteAdapter::s_getWorldImageCenter )

					//.def( "setScale", &Sprite::setScale )
					//.def( "getScale", &Sprite::getScale )
					.def( "setPercentVisibility", &Sprite::setPercentVisibility )
					.def_static( "setPercentVisibilityTo", &ScriptMethod::NodeAdapter::setPercentVisibilityTo )
					.def_static( "setPercentVisibilityStop", &ScriptMethod::NodeAdapter::setPercentVisibilityStop )
					.def( "setFlipX", &Sprite::setFlipX )
					.def( "setFlipY", &Sprite::setFlipY )
					.def( "getFlipX", &Sprite::getFlipX )
					.def( "getFlipY", &Sprite::getFlipY )
					.def( "getCenterAlign", &Sprite::getCenterAlign )
					.def( "setCenterAlign", &Sprite::setCenterAlign )
					.def( "disableTextureColor", &Sprite::disableTextureColor )
					.def( "setTextureMatrixOffset", &Sprite::setTextureMatrixOffset )
					;
				{
					pybind::proxy_<Animation, pybind::bases<Sprite, Animatable> >("Animation", false)
						.def( "setAnimationResource", &Animation::setAnimationResource )
						.def( "getAnimationResource", &Animation::getAnimationResource )
						.def( "getFrameCount", &Animation::getFrameCount )
						.def( "getFrameDelay", &Animation::getFrameDelay )
						.def( "setCurrentFrame", &Animation::setCurrentFrame )
						.def( "getCurrentFrame", &Animation::getCurrentFrame )
						;
				}

				//pybind::proxy_<RigidBody2D, pybind::bases<Node> >("RigidBody2D", false)
				//	.def( "applyForce", &RigidBody2D::applyForce )
				//	.def( "applyImpulse", &RigidBody2D::applyImpulse )
				//	.def( "applyConstantForce", &RigidBody2D::applyConstantForce )
				//	.def( "removeConstantForce", &RigidBody2D::removeConstantForce )
				//	.def( "setDirectionForce", &RigidBody2D::setDirectionForce )
				//	.def( "wakeUp", &RigidBody2D::wakeUp )
				//	.def( "getMass", &RigidBody2D::getMass )
				//	.def( "getLinearVelocity", &RigidBody2D::getLinearVelocity )
				//	.def( "setLinearVelocity", &RigidBody2D::setLinearVelocity )
				//	.def( "unsetLinearVelocity", &RigidBody2D::unsetLinearVelocity )
				//	.def( "setCollisionMask", &RigidBody2D::setCollisionMask )
				//	.def( "enableStabilization", &RigidBody2D::enableStabilization )
				//	;
				
				//pybind::proxy_<PhysicalBody2D, pybind::bases<Node> >("PhysicalBody2D", false)
				//	.def( "applyForce", &PhysicalBody2D::applyForce )
				//	.def( "applyImpulse", &PhysicalBody2D::applyImpulse )
				//	.def( "applyConstantForce", &PhysicalBody2D::applyConstantForce )
				//	.def( "removeConstantForce", &PhysicalBody2D::removeConstantForce )
				//	.def( "setDirectionForce", &PhysicalBody2D::setDirectionForce )
				//	.def( "wakeUp", &PhysicalBody2D::wakeUp )
				//	.def( "getMass", &PhysicalBody2D::getMass )
				//	.def( "getLinearVelocity", &PhysicalBody2D::getLinearVelocity )
				//	.def( "setLinearVelocity", &PhysicalBody2D::setLinearVelocity )
				//	.def( "unsetLinearVelocity", &PhysicalBody2D::unsetLinearVelocity )
				//	.def( "enableStabilization", &PhysicalBody2D::enableStabilization )
				//	.def( "setOrientation", &PhysicalBody2D::setOrientation )
				//	.def( "addShapeBox", &PhysicalBody2D::addShapeBox )
				//	.def( "addShapeCircle", &PhysicalBody2D::addShapeCircle )
				//	.def( "addShapeConvex", &PhysicalBody2D::addShapeConvex )
				//	.def( "createBody", &PhysicalBody2D::createBody )
				//	.def( "setFilterData", &PhysicalBody2D::setFilterData )
				//	.def( "setCollisionMask", &PhysicalBody2D::setCollisionMask )
				//	.def( "setCategoryBits", &PhysicalBody2D::setCategoryBits )
				//	.def( "setGroupIndex", &PhysicalBody2D::setGroupIndex )
				//	.def( "setLinearDumping", &PhysicalBody2D::setLinearDumping )
				//	.def( "setAngularDumping", &PhysicalBody2D::setAngularDumping )
				//	.def( "setAllowSleep", &PhysicalBody2D::setAllowSleep )
				//	.def( "setIsBullet", &PhysicalBody2D::setIsBullet )
				//	.def( "setFixedRotation", &PhysicalBody2D::setFixedRotation )
				//	.def( "getCollisionMask", &PhysicalBody2D::getCollisionMask )
				//	.def( "getCategoryBits", &PhysicalBody2D::getCategoryBits )
				//	.def( "getGroupIndex", &PhysicalBody2D::getGroupIndex )
				//	.def( "getLinearDumping", &PhysicalBody2D::getLinearDumping )
				//	.def( "getAngularDumping", &PhysicalBody2D::getAngularDumping )
				//	.def( "getAllowSleep", &PhysicalBody2D::getAllowSleep )
				//	.def( "getIsBullet", &PhysicalBody2D::getIsBullet )
				//	.def( "getFixedRotation", &PhysicalBody2D::getFixedRotation )
				//	.def( "getIsSensor", &PhysicalBody2D::getIsSensor )
				//	;
				
				//pybind::proxy_<TilePolygon, pybind::bases<RigidBody2D> >("TilePolygon", false)
				//	;

				pybind::proxy_<Movie, pybind::bases<Node, Animatable> >("Movie", false)
					.def( "setReverse", &Movie::setReverse )
					.def( "getReverse", &Movie::getReverse )
					.def( "setResourceMovie", &Movie::setResourceMovie )					
					.def( "getMovieSlot", &Movie::getMovieSlot )
					.def( "hasMovieSlot", &Movie::hasMovieSlot )
					;

				pybind::proxy_<MovieInternalObject, pybind::bases<Node> >("MovieInternalObject", false)
					;				

				pybind::proxy_<Video, pybind::bases<Node , Animatable> >("Video", false)
					.def("pause",&Video::pause )
					.def( "setVideoResource", &Video::setVideoResource )
					;

				pybind::proxy_<Window, pybind::bases<Node> >("Window", false)
					.def( "setClientSize", &Window::setClientSize )
					.def( "setClientSizeClip", &Window::setClientSizeClip )
					.def( "setClientSizeInTiles", &Window::setClientSizeInTiles )
					.def( "getClientSize", &Window::getClientSize )
					.def( "getWindowSize", &Window::getWindowSize )
					.def( "getTileSize", &Window::getTileSize )
					.def( "setResourceWindow", &Window::setResourceWindow )
					;

			}		

			pybind::def_function( "setCurrentScene", &ScriptMethod::setCurrentScene );
			pybind::def_function( "getCurrentScene", &ScriptMethod::getCurrentScene );
			 
			pybind::def_function( "createScene", &ScriptMethod::s_createScene );

			pybind::def_function( "setCamera2DPosition", &ScriptMethod::setCamera2DPosition );
			pybind::def_function( "getCamera2DPosition", &ScriptMethod::s_getCamera2DPosition );
			pybind::def_function( "setCamera2DDirection", &ScriptMethod::setCamera2DDirection );

			pybind::def_function( "createNode", &ScriptMethod::createNode );
			//pybind::def_function( "createNodeFromBinary", &ScriptMethod::createNodeFromBinary );
			pybind::def_function( "destroyNode", &ScriptMethod::destroyNode );

			pybind::def_function( "timing", &ScriptMethod::timing );
			pybind::def_function( "timingRemove", &ScriptMethod::timingRemove );
			

			pybind::def_function( "schedule", &ScriptMethod::schedule );
			pybind::def_function( "scheduleRemove", &ScriptMethod::scheduleRemove );
			pybind::def_function( "scheduleRemoveAll", &ScriptMethod::scheduleRemoveAll );
			pybind::def_function( "scheduleFreeze", &ScriptMethod::s_scheduleFreeze );
			pybind::def_function( "scheduleFreezeAll", &ScriptMethod::s_scheduleFreezeAll );
			pybind::def_function( "scheduleResumeAll", &ScriptMethod::scheduleResumeAll );
			pybind::def_function( "scheduleIsFreeze", &ScriptMethod::s_scheduleIsFreeze );
			pybind::def_function( "scheduleTime", &ScriptMethod::s_scheduleTime );

			pybind::def_function( "scheduleGlobal", &ScriptMethod::s_scheduleGlobal );
			pybind::def_function( "scheduleGlobalRemove", &ScriptMethod::s_scheduleGlobalRemove );
			pybind::def_function( "scheduleGlobalRemoveAll", &ScriptMethod::s_scheduleGlobalRemoveAll );
			pybind::def_function( "scheduleGlobalFreeze", &ScriptMethod::s_scheduleGlobalFreeze );
			pybind::def_function( "scheduleGlobalFreezeAll", &ScriptMethod::s_scheduleGlobalFreezeAll );
			pybind::def_function( "scheduleGlobalResumeAll", &ScriptMethod::s_scheduleGlobalResumeAll );
			pybind::def_function( "scheduleGlobalIsFreeze", &ScriptMethod::s_scheduleGlobalIsFreeze );
			pybind::def_function( "scheduleGlobalTime", &ScriptMethod::s_scheduleGlobalTime );


			pybind::def_function( "getMouseX", &ScriptMethod::getMouseX ); //deprecated
			pybind::def_function( "getMouseY", &ScriptMethod::getMouseY ); //deprecated

			pybind::def_function( "getCursorPosition", &ScriptMethod::s_getCursorPosition );

			pybind::def_function( "setArrow", &ScriptMethod::s_setArrow );
			pybind::def_function( "getArrow", &ScriptMethod::s_getArrow );

			pybind::def_function( "setArrowLayer", &ScriptMethod::s_setArrowLayer );

			pybind::def_function( "directResourceCompile", &ScriptMethod::directResourceCompile );
			pybind::def_function( "directResourceRelease", &ScriptMethod::directResourceRelease );
			//pybind::def_function( "directResourceFileCompile", &ScriptMethod::s_directResourceFileCompile );
			//pybind::def_function( "deferredResourceFileCompile", &ScriptMethod::s_deferredResourceFileCompile );
			//pybind::def_function( "directResourceFileRelease", &ScriptMethod::s_directResourceFileRelease );
			pybind::def_function( "getResourceReference", &ScriptMethod::s_getResourceReference );
			//pybind::def_function( "loadResourceImage", &ScriptMethod::s_loadResourceImage );
			//pybind::def_function( "loadImageResources", &ScriptMethod::s_loadImageResources );

			pybind::def_function( "quitApplication", &ScriptMethod::quitApplication );
			//pybind::def_function( "createShot", &ScriptMethod::createShot );
			pybind::def_function( "setFullscreenMode", &ScriptMethod::setFullscreenMode );
			pybind::def_function( "getFullscreenMode", &ScriptMethod::s_getFullscreenMode );
			pybind::def_function( "renderOneFrame", &ScriptMethod::renderOneFrame );
			pybind::def_function( "writeImageToFile", &ScriptMethod::writeImageToFile );
			pybind::def_function( "createImageResource", &ScriptMethod::s_createImageResource );
			//pybind::def_function( "createFolder", &ScriptMethod::createFolder );
			//pybind::def_function( "deleteFolder", &ScriptMethod::deleteFolder );
			pybind::def_function( "minimizeWindow", &ScriptMethod::minimizeWindow );
			pybind::def_function( "setMouseBounded", &ScriptMethod::s_setMouseBounded );
			pybind::def_function( "getMouseBounded", &ScriptMethod::s_getMouseBounded );

			pybind::def_function( "getCurrentResolution", &ScriptMethod::s_getCurrentResolution );
			pybind::def_function( "getContentResolution", &ScriptMethod::s_getContentResolution );
			pybind::def_function( "getHotSpotImageSize", &ScriptMethod::s_getHotSpotImageSize );

			//pybind::def_function( "setBlow", &ScriptMethod::setBlow );
			//pybind::def_function( "getBlow", &ScriptMethod::getBlow );
			//pybind::def_function( "setEnoughBlow", &ScriptMethod::setEnoughBlow );
			//pybind::def_function( "setBlowCallback", &ScriptMethod::setBlowCallback );

			pybind::def_function( "isKeyDown", &ScriptMethod::s_isKeyDown );
			pybind::def_function( "isMouseButtonDown", &ScriptMethod::s_isMouseButtonDown );
			pybind::def_function( "isInViewport", &ScriptMethod::s_isInViewport );
			//pybind::def_function( "getResourceCount", &ScriptMethod::s_getResourceCount );
			pybind::def_function( "enableTextureFiltering", &ScriptMethod::s_enableTextureFiltering );
			pybind::def_function( "isTextureFilteringEnabled", &ScriptMethod::s_isTextureFilteringEnabled );

			pybind::def_function( "existText", &ScriptMethod::s_existText );

			pybind::def_function( "pickHotspot", &ScriptMethod::s_pickHotspot );
			
			pybind::def_function( "blockInput", &ScriptMethod::s_blockInput );
			pybind::def_function( "setInputMouseButtonEventBlock", &ScriptMethod::s_setInputMouseButtonEventBlock );
			pybind::def_function( "getInputMouseButtonEventBlock", &ScriptMethod::s_getInputMouseButtonEventBlock );
			
			

			//pybind::def_function( "getParam", &ScriptMethod::s_getParam );
			pybind::def_function( "addJoin", &ScriptMethod::s_addJoin );
			pybind::def_function( "removeJoin", &ScriptMethod::s_removeJoin );
			pybind::def_function( "isJoin", &ScriptMethod::s_isJoin );			
			pybind::def_function( "getJoins", &ScriptMethod::s_getJoins );

			pybind::def_function( "loadPlugin", &ScriptMethod::s_loadPlugin );

			//pybind::def_function( "loadText", &ScriptMethod::s_loadText );

			pybind::def_function( "getRenderCamera2D", &ScriptMethod::s_getRenderCamera2D );

			pybind::def_function( "testHotspot", &ScriptMethod::s_testHotspot);
			pybind::def_function( "polygon_wm", &ScriptMethod::s_polygon_wm);

			pybind::def_function( "createAffector", &ScriptMethod::createAffector );			
			pybind::def_function( "removeAllEvent", &ScriptMethod::removeAllEvent );

			pybind::def_function( "showKeyboard", &ScriptMethod::showKeyboard );			
			pybind::def_function( "hideKeyboard", &ScriptMethod::hideKeyboard );

			pybind::def_function( "hasResource", &ScriptMethod::hasResource );

			pybind::def_function( "removeCurrentScene", &ScriptMethod::removeCurrentScene );

			pybind::def_function( "setDefaultResourceFontName", &ScriptMethod::setDefaultResourceFontName );

			pybind::def_function( "addMouseMoveHandler", &ScriptMethod::s_addMouseMoveHandler );
			pybind::def_function( "removeMouseMoveHandler", &ScriptMethod::s_removeMouseMoveHandler );
			
			pybind::def_function( "addMouseButtonHandler", &ScriptMethod::s_addMouseButtonHandler );
			pybind::def_function( "removeMouseButtonHandler", &ScriptMethod::s_removeMouseButtonHandler );

			pybind::def_function( "visitChild", &ScriptMethod::s_visitChild );

			pybind::def_function( "visitCompiledResources", &ScriptMethod::s_visitCompiledResources );

			pybind::def_function( "validResource", &ScriptMethod::s_validResource );

			//pybind::def_function( "createDistanceJoint", &ScriptMethod::s_createDistanceJoint );
			//pybind::def_function( "createHingeJoint", &ScriptMethod::s_createHingeJoint );
			//pybind::def_function( "createMouseJoint", &ScriptMethod::s_createMouseJoint );
			//
			//pybind::def_function( "createGearJoint", &ScriptMethod::s_createGearJoint );
			//pybind::def_function( "createPrismaticJoint", &ScriptMethod::s_createPrismaticJoint );
			//pybind::def_function( "createPulleyJoint", &ScriptMethod::s_createPulleyJoint );
			//pybind::def_function( "createRopeJoint", &ScriptMethod::s_createRopeJoint );
			//pybind::def_function( "createWheelJoint", &ScriptMethod::s_createWheelJoint );
			//pybind::def_function( "destroyJoint", &ScriptMethod::s_destroyJoint );
			//pybind::def_function( "physicsMouseMove", &ScriptMethod::s_physicsMouseMove );

			pybind::def_function( "getPlatformName", &ScriptMethod::s_getPlatformName );
			pybind::def_function( "getTimingFactor", &ScriptMethod::s_getTimingFactor );
			pybind::def_function( "setTimingFactor", &ScriptMethod::s_setTimingFactor );

			pybind::def_function( "addHomeless", &ScriptMethod::s_addHomeless );
			pybind::def_function( "cancelTask", &ScriptMethod::s_cancelTask );
			pybind::def_function( "joinTask", &ScriptMethod::s_joinTask );
			
			//For Astralax
			pybind::def_function( "visitResourceEmitterContainer", &ScriptMethod::s_visitResourceEmitterContainer );

			pybind::def_function( "getNullObjectsFromResourceVideo", &ScriptMethod::s_getNullObjectsFromResourceVideo );

			pybind::def_function( "getMovieDuration", &ScriptMethod::s_getMovieDuration );

			pybind::def_function( "addEntityPrototype", &ScriptMethod::s_addEntityPrototype );
			pybind::def_function( "addScenePrototype", &ScriptMethod::s_addScenePrototype );
			pybind::def_function( "addArrowPrototype", &ScriptMethod::s_addArrowPrototype );

			pybind::def_function( "getLowContentViewport", &ScriptMethod::s_getLowContentViewport );			

			pybind::def_function( "destroyArrow", &ScriptMethod::s_destroyArrow );
			pybind::def_function( "getGameParam", &ScriptMethod::s_getGameParam );
			pybind::def_function( "hasGameParam", &ScriptMethod::s_hasGameParam );
			pybind::def_function( "openUrlInDefaultBrowser", &ScriptMethod::s_openUrlInDefaultBrowser );
			
		}
	}
}
