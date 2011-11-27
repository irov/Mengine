
#	include "ScriptWrapper.h"

#	include "InputEngine.h"
#	include "NodeManager.h"
#	include "FileEngine.h"
#	include "Scene.h"
#	include "Game.h"

#	include "Consts.h"

#	include "ScriptClassWrapper.h"

#	include "ScriptEngine.h"
#	include "ScheduleManager.h"
#	include "ParamManager.h"

#	include "TextManager.h"
#	include "ArrowManager.h"
#	include "SceneManager.h"
#	include "AccountManager.h"

#	include "EventManager.h"

#	include "ResourceManager.h"
#	include "ResourceImageDynamic.h"
#	include "ResourceImageDefault.h"
#	include "ResourceHotspotImage.h"

#	include "Player.h"
#	include "Application.h"
#	include "MousePickerSystem.h"

#	include "Amplifier.h"
#	include "Sprite.h"
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
#	include "Layer2DTexture.h"

#	include "RigidBody2D.h"

#	include "TilePolygon.h"

#	include "Movie.h"
#	include "Video.h"

#	include "Window.h"
#	include "HotSpotImage.h"
#	include "Mesh_40_30.h"

#	include "Entity.h"

//#	include "DiscreteEntity.h"

#	include "SoundEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"

#	include "TimingManager.h"

#	include "Identity.h"

#	include "Affector.h"

//#	include "Layer3D.h"

//#	include "FFCamera.h"
#	include "SceneNode3D.h"
#	include "Camera3D.h"
#	include "RenderMesh.h"

#	include "TaskManager.h"
#	include "TaskDeferredLoading.h"

#	include "Core/Polygon.h"

#	include "Utils/Math/angle.h"
#	include "Utils/Math/vec4.h"
#	include "Utils/Math/mat3.h"
#	include "Utils/Math/mat4.h"
#	include "Utils/Math/quat.h"
#	include "Utils/Math/clamp.h"

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
		namespace PolygonAdapter
		{
			static PyObject * s_getPoints( Polygon * _polygon )
			{				
				PyObject * py_list_vec2f = pybind::list_new(0);

				const Polygon::ring_type & ring = _polygon->outer();

				for( Polygon::ring_type::const_iterator
					it = ring.begin(),
					it_end = ring.end();
				it != it_end;
				++it )
				{
					PyObject * py_vec2f = pybind::ptr(*it);

					pybind::list_appenditem( py_list_vec2f, py_vec2f );

					pybind::decref( py_vec2f );
				}

				return py_list_vec2f;
			}
		}

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

				const mt::mat3f & wm = _hs->getWorldMatrix();

				mt::vec2f world_pc;
				mt::mul_v2_m3(world_pc, pc, wm);

				return world_pc;
			}
			
			static Polygon s_getWorldPolygon( HotSpot * _hs )
			{
				const Polygon & polygon = _hs->getPolygon();

				const mt::mat3f & wm = _hs->getWorldMatrix();

				Polygon pwm;
				polygon_wm(pwm, polygon, wm);

				return pwm;
			}
		}

		static Polygon s_polygon_wm( const Polygon & _polygon, const mt::mat3f & _wm )
		{
			Polygon polygon;
			polygon_wm(polygon, _polygon, _wm);

			return polygon;
		}

		static bool s_testHotspot( HotSpot * _left, HotSpot * _right )
		{
			const Polygon & left_poligon = _left->getPolygon();
			const mt::mat3f & left_wm = _left->getWorldMatrix();

			const Polygon & right_poligon = _right->getPolygon();
			const mt::mat3f & right_wm = _right->getWorldMatrix();
			
			Polygon left_polygon_wm;

			polygon_wm(left_polygon_wm, left_poligon, left_wm);

			Polygon right_polygon_wm;

			polygon_wm(right_polygon_wm, right_poligon, right_wm);

			bool result = boost::geometry::intersects( left_polygon_wm, right_polygon_wm );
			
			return result;
			
			//return _left->testPolygon( left_wm, right_poligon, right_wm );
		}

		static bool s_loadPlugin( const String & _pluginName, const TMapParam & _param )
		{
			bool result = Application::get()
				->loadPlugin( _pluginName, _param );

			return result;
		}

		//static bool s_loadText( const ConstString & _name )
		//{
		//	bool result = TextManager::get()
		//		->loadResourceFile( _name );

		//	return result;
		//}

		static PyObject * s_getParam( const ConstString & _name )
		{
			TVectorParams params;
			if( ParamManager::get()
				->getParam( _name, params ) == false )
			{
				MENGE_LOG_ERROR("Menge.getParam: %s not found"
					, _name.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_param = pybind::ptr( params );

			return py_param;
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
			bool update( std::size_t _id, float _timing )
			{
				EventManager::get()
					->addEventFormat( EVENT_TIMING, m_script, "(if)", _id, _timing );
				
				return false;
			}

		protected:
			PyObject * m_script;
		};

		static std::size_t timing( bool _portion, bool _global, float _timing, PyObject * _script )
		{
			TimingManager * tm = Player::get()
				->getTimingManager();

			TimingListener * listener = new MyTimingListener(_script);

			std::size_t id = tm->timing( _portion, _global, _timing, listener );

			return id;
		}

		static void timingRemove( std::size_t _id )
		{
			TimingManager * tm = Player::get()
				->getTimingManager();

			tm->remove( _id );
		}

		static std::size_t schedule( float _timing, PyObject * _script )
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			std::size_t id = sm->schedule( _timing, _script );

			return id;
		}

		static void scheduleRemove( std::size_t _id )
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			sm->remove( _id );
		}

		static void scheduleRemoveAll()
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			sm->removeAll();
		}	

		static void s_scheduleFreeze( std::size_t _id, bool _freeze )
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

		static bool s_scheduleIsFreeze( std::size_t _id )
		{
			ScheduleManager* sm = Player::get()->getScheduleManager();
			return sm->isFreeze( _id );
		}

		static float s_scheduleTime( std::size_t _id )
		{
			ScheduleManager* sm = Player::get()->getScheduleManager();
			float time = sm->time( _id );
			
			return time;
		}

		static float getMouseX()
		{
			const Resolution& contRes = Game::get()->getContentResolution();
			float mx = Player::get()->getArrow()->getLocalPosition().x;
			mx = mt::clamp( 0.0f, mx, static_cast<float>( contRes.getWidth() ) );
			return mx;
		}

		static float getMouseY()
		{
			const Resolution& contRes = Game::get()->getContentResolution();
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

		static bool s_hasScene( const ConstString & _name )
		{
			bool result = SceneManager::get()
				->hasScene( _name );

			return result;
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
			return Game::get()
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

			node->loaded();

			Game::get()
				->addHomeless( node );

			return node;
		}

		static PyObject * createNodeFromBinary( const ConstString & _name, const ConstString & _tag, const ConstString & _binary )
		{
			Node * node = NodeManager::get()
				->createNodeFromBinary( _name, _tag, _binary );

			if( node == 0 )
			{
				return pybind::ret_none();
			}

			Game::get()
				->addHomeless( node );

			PyObject * py_embedding = node->getEmbed();

			if( py_embedding == 0 )
			{
				node->destroy();
				return pybind::ret_none();
			}

			pybind::incref( py_embedding );

			return py_embedding;
		}

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

		static void directResourceRelease( const ConstString& _nameResource )
		{
			ResourceManager::get()
				->directResourceRelease( _nameResource );
		}

		static void s_directResourceFileCompile( const ConstString& _category, const ConstString& _group )
		{
			ResourceManager::get()
				->directResourceFileCompile( _category, _group );
		}

		static void s_deferredResourceFileCompile( PyObject* _pycategory, PyObject* _resourceFiles, PyObject* _progressCallback )
		{
			if( pybind::string_check( _pycategory ) == false )
			{
				return;
			}

			ConstString category = pybind::extract<ConstString>( _pycategory );

			TVectorConstString resourceFiles;
			if( pybind::string_check( _resourceFiles ) == true )
			{
				ConstString resourceFile = pybind::extract<ConstString>( _resourceFiles );
				resourceFiles.push_back( resourceFile );
			}
			else if( pybind::list_check( _resourceFiles ) == true )
			{
				std::size_t listSize = pybind::list_size( _resourceFiles );
				for( std::size_t i = 0; i != listSize; ++i )
				{
					PyObject* listItem = pybind::list_getitem( _resourceFiles, i );
					if( pybind::string_check( listItem ) == false )
					{
						MENGE_LOG_ERROR( "Error: (Menge.deferredResourceFileCompile) invalid argument" );
						return;
					}
					ConstString resourceFile = pybind::extract<ConstString>( listItem );
					resourceFiles.push_back( resourceFile );
				}
			}
			else
			{
				MENGE_LOG_ERROR( "Error: (Menge.deferredResourceFileCompile) invalid argument" );
				return;
			}

			TaskDeferredLoading* task = 
				new TaskDeferredLoading( category, resourceFiles, _progressCallback );

			TaskManager::get()
				->addTask( task );
		}

		static void s_directResourceFileRelease( const ConstString& _category, const ConstString& _group )
		{
			ResourceManager::get()
				->directResourceFileRelease( _category, _group );
		}

		static PyObject * createShot( const ConstString& _name, const mt::vec2f & _min,  const mt::vec2f & _max )
		{
			mt::vec4f rect( _min, _max );

			ResourceImageDynamic * resourceImage 
				= ResourceManager::get()->getResourceT<ResourceImageDynamic>( _name );

			if( resourceImage == NULL )
			{
				ConstString group;

				Account * acc = AccountManager::get()
					->getCurrentAccount();

				if( acc != 0 )
				{
					group = acc->getFolder();
				}

				resourceImage = ResourceManager::get()
					->createResourceT<ResourceImageDynamic>( Consts::get()->c_user, group, _name, Consts::get()->c_ResourceImageDynamic );

				//FIXME
				Texture* texture
					= RenderEngine::get()->createTexture( _name, 
					::floorf( rect[2] - rect[0] + 0.5f ), 
					::floorf( rect[3] - rect[1] + 0.5f ), PF_R8G8B8 );

				resourceImage->setRenderImage( texture );
			}

			Texture* image = resourceImage->getTexture( 0 );

			//Holder<Application>::get()->update( 0.0f );
			Game::get()->tick(0.0f);

			RenderEngine::get()->beginScene();

			Game::get()->render();

			RenderEngine::get()->endScene();

			Application::get()->screenshot( image, mt::vec4f( _min.x, _min.y, _max.x, _max.y) );


			//image->writeToFile( "bl.bmp" );

			ConstString name("shotSprite");

			Sprite * node = NodeManager::get()
				->createNodeT<Sprite>( name, Consts::get()->c_Sprite, Consts::get()->c_builtin_empty );

			if( node == 0 )
			{
				return pybind::ret_none();
			}

			node->setImageResource( _name );

			node->enable();

			Game::get()
				->addHomeless( node );

			PyObject * py_embedding = node->getEmbed();

			if( py_embedding == 0 )
			{
				node->destroy();
				return pybind::ret_none();
			}

			pybind::incref( py_embedding );

			return py_embedding;		
		}

		static void setFullscreenMode( bool _fullscreen )
		{
			Application::get()->setFullscreenMode( _fullscreen );
		}

		static bool s_getFullscreenMode()
		{
			return Application::get()->getFullscreenMode();
		}

		class ScriptResourceManagerListener
			: public ResourceManagerListener
		{
		public:
			ScriptResourceManagerListener( PyObject* _eventLoaded, PyObject* _eventUnLoaded )
				: m_eventLoaded(_eventLoaded)
				, m_eventUnLoaded(_eventUnLoaded)
			{
			}

			~ScriptResourceManagerListener()
			{
				pybind::decref(m_eventLoaded);
				pybind::decref(m_eventUnLoaded);
			}

		public:
			void onResourceLoaded( const ConstString& _name ) override
			{
				if( m_eventLoaded == 0 )
				{
					return;
				}

				String nameAnsi;

				Application::get()
					->utf8ToAnsi( Helper::to_str(_name), nameAnsi );

				ScriptEngine::get()
					->callFunction( m_eventLoaded, "(s)", nameAnsi.c_str() );
			}

			void onResourceUnLoaded( const ConstString& _name ) override
			{
				if( m_eventLoaded == 0 )
				{
					return;
				}

				String nameAnsi;

				Application::get()
					->utf8ToAnsi( Helper::to_str(_name), nameAnsi );

				ScriptEngine::get()
					->callFunction( m_eventUnLoaded, "(s)", nameAnsi.c_str() );			
			}

		protected:
			PyObject * m_eventLoaded;
			PyObject * m_eventUnLoaded;
		};

		static void addResourceListener( PyObject* _pylistener )
		{
			PyObject * eventLoaded = 0;
			PyObject * eventUnLoaded = 0;

			if( ScriptEngine::get()
				->hasModuleFunction( _pylistener, ("onHandleResourceLoaded") ) == true )
			{
				eventLoaded = ScriptEngine::get()
					->getModuleFunction( _pylistener, ("onHandleResourceLoaded") );
			}

			if( ScriptEngine::get()
				->hasModuleFunction( _pylistener, ("onHandleResourceUnLoaded") ) == true )
			{
				eventUnLoaded = ScriptEngine::get()
					->getModuleFunction( _pylistener, ("onHandleResourceUnLoaded") );
			}

			ScriptResourceManagerListener * listener = new ScriptResourceManagerListener(eventLoaded, eventUnLoaded);

			ResourceManager::get()
				->addListener( listener );
		}

		static void renderOneFrame()
		{
			RenderEngine::get()->beginScene();
			Game::get()->render();
			RenderEngine::get()->endScene();
			RenderEngine::get()->swapBuffers();
		}

		static void writeImageToFile( const ConstString& _resource, int _frame, const String& _filename )
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

			Texture * img = resource->getTexture( _frame );

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

		static void s_createImageResource( const ConstString& _resourceName, const ConstString& _pakName, const ConstString& _filename )
		{
			ResourceImageDefault* resImage = ResourceManager::get()
				->getResourceT<ResourceImageDefault>( _resourceName );

			if( resImage == NULL )
			{
				resImage = ResourceManager::get()
					->createResourceT<ResourceImageDefault>( _pakName, Consts::get()->c_builtin_empty, _resourceName, Consts::get()->c_ResourceImageDefault );
			}

			resImage->addImagePath( _filename, mt::vec2f(-1.f,-1.f) );
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
		static bool setBlow( bool _active )
		{
			return SoundEngine::get()
				->setBlow( _active );
		}
		static float getBlow()
		{
			return SoundEngine::get()
				->getBlow();
		}
		static void setEnoughBlow( float _enoughBlow )
		{
			SoundEngine::get()
				->setEnoughBlow( _enoughBlow );
		}

		class PySoundSulkCallback
			: public SoundSulkCallback
		{
		public:
			PySoundSulkCallback( PyObject * _callback )
				: m_callback(_callback)
			{
				pybind::incref( m_callback );
			}

			~PySoundSulkCallback()
			{
				pybind::decref( m_callback );
			}

			void blow( float _blow ) override
			{
				pybind::call( m_callback, "(f)", _blow );
			}

		protected:
			PyObject * m_callback;
		};

		static void setBlowCallback( PyObject * _sulkcallback )
		{
			SoundEngine::get()
				->setSulkCallback( new PySoundSulkCallback( _sulkcallback ) );
		}

		//static PhysicJoint2DInterface* s_createDistanceJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offset1, const mt::vec2f& _offset2, bool _collideBodies )
		//{
		//	return PhysicEngine2D::get()->createDistanceJoint( _body1, _body2, _offset1, _offset2, _collideBodies );
		//}

		//static PhysicJoint2DInterface* s_createHingeJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offset1, const mt::vec2f& _limits, bool _collideBodies )
		//{
		//	return PhysicEngine2D::get()->createHingeJoint( _body1, _body2, _offset1, _limits, _collideBodies );
		//}

		//static PhysicJoint2DInterface* s_createMouseJoint( RigidBody2D* _body, int _x, int _y )
		//{
		//	return PhysicEngine2D::get()->createMouseJoint( _body, _x, _y );
		//}

		//static void s_destroyJoint( PhysicJoint2DInterface* _joint )
		//{
		//	return PhysicEngine2D::get()->destroyJoint( _joint );
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

		static size_t s_getResourceCount( const ConstString& _category, const ConstString& _group )
		{
			return ResourceManager::get()
				->getResourceCount( _category, _group );
		}

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

				mt::vec2f imageCenter( imageSize.x * 0.5f, imageSize.y * 0.5f );

				return imageCenter;
			}

			static mt::vec2f s_getWorldImageCenter( Sprite * _sprite )
			{
				mt::vec2f imageCenter = s_getLocalImageCenter( _sprite );

				const mt::mat3f & wm = _sprite->getWorldMatrix();

				mt::vec2f imageCenter_wm;
				mt::mul_v2_m3( imageCenter_wm, imageCenter, wm );

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
				_node->setLinearSpeed( mt::zero_v2 );
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t velocityTo( Node * _node, float _speed, const mt::vec2f& _dir, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				moveStop( _node );

				Affector * affector = 
					NodeAffectorCreator::newNodeAffectorAccumulateLinear(
					_cb, ETA_POSITION, _node, &Node::setLocalPosition
					, _node->getLocalPosition(), _dir, _speed, &mt::length_v2
					);

				mt::vec2f linearSpeed = _dir * _speed;
				_node->setLinearSpeed( linearSpeed );

				std::size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t moveTo( Node * _node, float _time, const mt::vec2f& _point, PyObject* _cb )
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
					, &mt::length_v2 
					);

				float invTime = 1.0f / _time;
				mt::vec2f linearSpeed = ( _point - _node->getLocalPosition() ) * invTime;

				_node->setLinearSpeed( linearSpeed );
				
				std::size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t accMoveTo( Node * _node, float _time, const mt::vec2f& _point, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				mt::vec2f linearSpeed = _node->getLinearSpeed();

				moveStop( _node );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateQuadratic(
					_cb, ETA_POSITION, _node, &Node::setLocalPosition
					, _node->getLocalPosition(), _point, linearSpeed, _time
					, &mt::length_v2
					);
				
				std::size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t bezier2To( Node * _node
				, float _time
				, const mt::vec2f& _point1
				, const mt::vec2f& _point2
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
					, &mt::length_v2
					);

				std::size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t bezier3To( Node * _node
									    , float _time
									    , const mt::vec2f& _point1
									    , const mt::vec2f& _point2
									    , const mt::vec2f& _point3
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
					, &mt::length_v2
					);

				std::size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static void angleStop( Node * _node )
			{
				_node->stopAffectors( ETA_ANGLE );
				_node->setAngularSpeed(0.f);
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t angleTo( Node * _node, float _time, float _angle, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				angleStop( _node );

				float angle = _node->getAngle();

				float correct_angle_from = angle;
				float correct_angle_to = _angle;
				//mt::angle_correct_interpolate_from_to( angle, _angle, correct_angle_from, correct_angle_to );

				Affector* affector =
					NodeAffectorCreator::newNodeAffectorInterpolateLinear(
					_cb, ETA_ANGLE, _node, &Node::setAngle
					, correct_angle_from, correct_angle_to, _time
					, &fabsf 
					);				

				float invTime = 1.0f / _time;
				float angularSpeed = abs( correct_angle_from - correct_angle_to ) * invTime;

				_node->setAngularSpeed( angularSpeed );				
				std::size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t accAngleTo( Node * _node, float _time, float _angle, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				float angularSpeed = _node->getAngularSpeed();

				angleStop( _node );

				float angle = _node->getAngle();

				float correct_angle_from = angle;
				float correct_angle_to = _angle;
				//mt::angle_correct_interpolate_from_to( angle, _angle, correct_angle_from, correct_angle_to );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateQuadratic(
					_cb, ETA_ANGLE, _node, &Node::setAngle
					, correct_angle_from, correct_angle_to, angularSpeed, _time
					, &fabsf
					);				

				std::size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static void scaleStop( Node * _node )
			{
				_node->stopAffectors( ETA_SCALE );
			}	
			//////////////////////////////////////////////////////////////////////////
			static std::size_t scaleTo( Node * _node, float _time, const mt::vec2f& _scale, PyObject* _cb )
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
					, &mt::length_v2
					);

				std::size_t id = _node->addAffector( affector );
				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static void colorStop( Node * _node )
			{
				_node->stopAffectors( ETA_COLOR );
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t colorTo( Node * _node, float _time, const ColourValue& _color, PyObject* _cb )
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

				std::size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t alphaTo( Node * _node, float _time, float _alpha, PyObject* _cb )
			{
				if( _node->isActivate() == false )
				{
					return 0;
				}

				ColourValue color = _node->getLocalColor();
				color.setA( _alpha );

				std::size_t id = colorTo( _node, _time, color, _cb );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t setPercentVisibilityTo( Sprite * _sprite, float _time, const mt::vec4f& _percent, PyObject* _cb )
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

				std::size_t id = _sprite->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static void setPercentVisibilityStop( Sprite * _sprite )
			{
				_sprite->stopAffectors( ETA_VISIBILITY );
			}
		};

		//////////////////////////////////////////////////////////////////////////
		std::string vec2f_repr( PyObject * _obj, mt::vec2f * _v )
		{
			std::stringstream ss;
			ss << "<vec2f: " << _v->x << ", " << _v->y << ">";
			return ss.str();
		}

		float vec2_sequence( mt::vec2f * _vec, std::size_t _index )
		{
			if( _index > 2 )
			{
				pybind::throw_exception();
			}

			return _vec->operator [] (_index);
		}

		float vec4_sequence( mt::vec4f * _vec, std::size_t _index )
		{
			if( _index > 4 )
			{
				pybind::throw_exception();
			}

			return _vec->operator [] (_index);
		}

		//////////////////////////////////////////////////////////////////////////
		std::string color_repr( PyObject * _obj, ColourValue * _v )
		{
			std::stringstream ss;
			ss << "<color: " << _v->getA() << ", " << _v->getR() << ", " << _v->getG() << ", " << _v->getB() << ">";
			return ss.str();
		}
		//////////////////////////////////////////////////////////////////////////
		bool vec2f_convert( PyObject * _obj, void * _place )
		{
			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 2 )
				{
					return false;
				}

				mt::vec2f * impl = (mt::vec2f *)_place;

				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );

				impl->x = pybind::extract<float>(i0);
				impl->y = pybind::extract<float>(i1);

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 2 )
				{
					return false;
				}

				mt::vec2f * impl = (mt::vec2f *)_place;

				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );

				impl->x = pybind::extract<float>(i0);
				impl->y = pybind::extract<float>(i1);

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		bool vec4f_convert( PyObject * _obj, void * _place )
		{
			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 4 )
				{
					return false;
				}

				mt::vec4f * impl = (mt::vec4f *)_place;

				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );
				PyObject * i2 = pybind::tuple_getitem( _obj, 2 );
				PyObject * i3 = pybind::tuple_getitem( _obj, 3 );

				impl->x = pybind::extract<float>(i0);
				impl->y = pybind::extract<float>(i1);
				impl->z = pybind::extract<float>(i2);
				impl->w = pybind::extract<float>(i3);

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 4 )
				{
					return false;
				}

				mt::vec4f * impl = (mt::vec4f *)_place;

				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );
				PyObject * i2 = pybind::list_getitem( _obj, 2 );
				PyObject * i3 = pybind::list_getitem( _obj, 3 );

				impl->x = pybind::extract<float>(i0);
				impl->y = pybind::extract<float>(i1);
				impl->z = pybind::extract<float>(i2);
				impl->w = pybind::extract<float>(i3);

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool Polygon_convert( PyObject * _obj, void * _place )
		{
			if( pybind::list_check( _obj ) == false )
			{
				return false;
			}

			Polygon * polygon = (Polygon*)_place;

			std::size_t size = pybind::list_size( _obj );

			for( std::size_t i = 0; i != size; ++i )
			{
				PyObject * py_point = pybind::list_getitem(_obj, i);

				mt::vec2f point = pybind::extract<mt::vec2f>(py_point);

				boost::geometry::append(*polygon, point);
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool color_convert( PyObject * _obj, void * _place )
		{
			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 4 )
				{
					return false;
				}

				ColourValue * impl = (ColourValue *)_place;

				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );
				PyObject * i2 = pybind::tuple_getitem( _obj, 2 );
				PyObject * i3 = pybind::tuple_getitem( _obj, 3 );

				impl->r = pybind::extract<float>(i0);
				impl->g = pybind::extract<float>(i1);
				impl->b = pybind::extract<float>(i2);
				impl->a = pybind::extract<float>(i3);
				
				impl->invalidate();

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 4 )
				{
					return false;
				}

				ColourValue * impl = (ColourValue *)_place;

				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );
				PyObject * i2 = pybind::list_getitem( _obj, 2 );
				PyObject * i3 = pybind::list_getitem( _obj, 3 );

				impl->r = pybind::extract<float>(i0);
				impl->g = pybind::extract<float>(i1);
				impl->b = pybind::extract<float>(i2);
				impl->a = pybind::extract<float>(i3);

				return true;
			}

			return false;
		}

		static bool resolution_convert( PyObject * _obj, void * _place )
		{
			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 2 )
				{
					return false;
				}

				Resolution * impl = (Resolution *)_place;

				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );

				std::size_t width = pybind::extract<std::size_t>(i0);
				std::size_t height = pybind::extract<std::size_t>(i1);

				impl->setWidth( width );
				impl->setHeight( height );

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 2 )
				{
					return false;
				}

				Resolution * impl = (Resolution *)_place;

				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );

				std::size_t width = pybind::extract<std::size_t>(i0);
				std::size_t height = pybind::extract<std::size_t>(i1);

				impl->setWidth( width );
				impl->setHeight( height );

				return true;
			}

			return false;
		}

		static bool Viewport_convert( PyObject * _obj, void * _place )
		{
			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 2 )
				{
					return false;
				}

				Viewport * impl = (Viewport *)_place;

				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );

				mt::vec2f begin = pybind::extract<mt::vec2f>(i0);
				mt::vec2f end = pybind::extract<mt::vec2f>(i1);

				impl->initialize( begin, end );

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 2 )
				{
					return false;
				}

				Viewport * impl = (Viewport *)_place;

				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );

				mt::vec2f begin = pybind::extract<mt::vec2f>(i0);
				mt::vec2f end = pybind::extract<mt::vec2f>(i1);

				impl->initialize( begin, end );

				return true;
			}

			return false;
		}

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
				//pybind::incref( py_embedding );
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

		static void removeCurrentScene()
		{
			Player::get()
				->removeCurrentScene();
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
	}

	static void classWrapping()
	{
		SCRIPT_CLASS_WRAPPING( Node );
		SCRIPT_CLASS_WRAPPING( Layer );
		SCRIPT_CLASS_WRAPPING( Layer2D );
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
		SCRIPT_CLASS_WRAPPING( Point );
		//SCRIPT_CLASS_WRAPPING( TilePolygon );
		SCRIPT_CLASS_WRAPPING( Video );
		//SCRIPT_CLASS_WRAPPING( FFCamera3D );
		//SCRIPT_CLASS_WRAPPING( DiscreteEntity );
		//SCRIPT_CLASS_WRAPPING( RigidBody3D );
		//SCRIPT_CLASS_WRAPPING( Layer3D );
		//SCRIPT_CLASS_WRAPPING( RigidBody2D );
		//SCRIPT_CLASS_WRAPPING( CapsuleController );
		//SCRIPT_CLASS_WRAPPING( SceneNode3D );
		//SCRIPT_CLASS_WRAPPING( Camera3D );
		//SCRIPT_CLASS_WRAPPING( RenderMesh );
		SCRIPT_CLASS_WRAPPING( Window );
		SCRIPT_CLASS_WRAPPING( HotSpotImage );
		//SCRIPT_CLASS_WRAPPING( Mesh_40_30 );
		SCRIPT_CLASS_WRAPPING( Camera2D );
		//SCRIPT_CLASS_WRAPPING( Layer2DTexture );
	}

	static struct extract_const_string_type
		: public pybind::type_cast_result<ConstString>
	{
		bool apply( PyObject * _obj, ConstString & _value ) override
		{
			if( pybind::string_check( _obj ) == true )
			{

				size_t ch_size;

				const char * ch_buff = pybind::string_to_char( _obj, ch_size );

				if( ch_size == 0 )
				{
					_value = Consts::get()->c_builtin_empty;
					
					return true;
				}		

				_value = ConstString( ch_buff, ch_size );
			}
			else if( pybind::unicode_check( _obj ) )
			{
				size_t ch_size;

				const char * ch_buff = pybind::unicode_to_utf8( _obj, ch_size );

				if( ch_size == 0 )
				{
					_value = Consts::get()->c_builtin_empty;

					return true;
				}
				
				_value = ConstString( ch_buff, ch_size );
			}
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( type_cast_result<ConstString>::TCastRef _value ) override
		{
			return pybind::string_from_char( _value.c_str(), _value.size() );
		}
	}s_extract_const_string_type;

	static struct extract_TVectorString_type
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
					pybind::extract<String>(py_string, key);

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

		PyObject * wrap( type_cast_result<TVectorString>::TCastRef _value ) override
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
	}s_extract_TVectorString_type;

	static struct extract_TVectorParams_type
		: public pybind::type_cast_result<TVectorParams>
	{
		bool apply( PyObject * _obj, TVectorParams & _value ) override
		{
			TVectorParams values;

			if( pybind::list_check( _obj ) == true )
			{
				size_t size = pybind::list_size( _obj );

				for( size_t it = 0; it != size; ++it )
				{
					PyObject * py_value = pybind::list_getitem( _obj, it );

					TVectorString param;
					pybind::extract<TVectorString>( py_value, param );

					_value.push_back( param );

					pybind::decref( py_value );
				}
			}
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( type_cast_result<TVectorParams>::TCastRef _value ) override
		{
			PyObject * py_param = pybind::list_new(0);

			for( TVectorParams::const_iterator
				it = _value.begin(),
				it_end = _value.end();
			it != it_end;
			++it )
			{
				PyObject * py_value = pybind::ptr( *it );

				pybind::list_appenditem( py_param, py_value );

				pybind::decref( py_value );
			}

			return py_param;
		}
	}s_extract_TVectorParams_type;

	static struct extract_TMapParam_type
		: public pybind::type_cast_result<TMapParam>
	{
		bool apply( PyObject * _obj, TMapParam & _param ) override
		{
			if( pybind::dict_check( _obj ) == false )
			{
				return false;
			}
			
			PyObject * py_items = pybind::dict_items( _obj );
			size_t size = pybind::list_size( py_items );

			for( size_t it = 0; it != size; ++it )
			{
				PyObject * py_tuple = pybind::list_getitem( py_items, it );

				PyObject * py_key = pybind::tuple_getitem( py_tuple, 0 );
				PyObject * py_value = pybind::tuple_getitem( py_tuple, 1 );

				String key;
				String value;
				pybind::extract<String>(py_key, key);
				pybind::extract<String>(py_value, value );

				_param.insert( std::make_pair( key, value ) );
			}

			pybind::decref( py_items );

			return true;
		}

		PyObject * wrap( type_cast_result<TMapParam>::TCastRef _value ) override
		{
			PyObject * py_param = pybind::dict_new();

			for( TMapParam::const_iterator
				it = _value.begin(),
				it_end = _value.end();
			it != it_end;
			++it )
			{
				PyObject * py_value = pybind::ptr( it->second );

				pybind::dict_set( py_param, it->first.c_str(), py_value );

				pybind::decref( py_value );
			}

			return py_param;
		}
	}s_extract_TMapParam_type;


	void ScriptWrapper::finalize()
	{
		//delete st;
	}

	//REGISTER_SCRIPT_CLASS( Menge, Node, Base )
	void ScriptWrapper::nodeWrap()
	{
		classWrapping();
		
		pybind::interface_<Join>("Join")
			.def("getLeft", &Join::getLeft)
			.def("getRight", &Join::getRight)
			.def("getOffset", &Join::getOffset)
			;

		//pybind::class_<ScriptMethod::NodeGetChild>( "NodeGetChild" )
		//	.def_getattro( &ScriptMethod::NodeGetChild::getChild )
		//	.def_mapping( &ScriptMethod::NodeGetChild::getChild )
		//	;

		pybind::class_<mt::vec2f>("vec2f")
			.def( pybind::init<float,float>() )
			.def_convert( &ScriptMethod::vec2f_convert )
			.def_static_sequence( &ScriptMethod::vec2_sequence )
			.def_repr( &ScriptMethod::vec2f_repr )

			.def_member( "x", &mt::vec2f::x )
			.def_member( "y", &mt::vec2f::y )
			//.attr( "x", &vec2f::x )
			//.def( boost::python::init<float,float>() )
			//.def( boost::python::self + boost::python::self )	// __add__
			//.def( boost::python::self - boost::python::self )          // __radd__
			//.def( boost::python::self * float() )           // __sub__
			//.def( boost::python::self / float() )          // __sub__
			//.def( float() * boost::python::self )         // __iadd__
			//.def( boost::python::self += boost::python::self )
			//.def( boost::python::self -= boost::python::self )
			//.def( boost::python::self *= float() )
			//.def( boost::python::self /= float() )
			;

		pybind::class_<mt::vec3f>("vec3f")
			.def( pybind::init<float,float,float>() )
			.def_member( "x", &mt::vec3f::x )
			.def_member( "y", &mt::vec3f::y )
			.def_member( "z", &mt::vec3f::z )
			;

		pybind::class_<mt::vec4f>("vec4f")
			.def( pybind::init<float,float,float,float>() )
			.def_convert( &ScriptMethod::vec4f_convert )
			.def_static_sequence( &ScriptMethod::vec4_sequence )

			.def_member( "x", &mt::vec4f::x )
			.def_member( "y", &mt::vec4f::y )
			.def_member( "z", &mt::vec4f::z )
			.def_member( "w", &mt::vec4f::w )
			;

		pybind::class_<mt::quatf>("quatf")
			.def( pybind::init<float,float,float,float>() )
			.def_member( "w", &mt::quatf::w )
			.def_member( "x", &mt::quatf::x )
			.def_member( "y", &mt::quatf::y )
			.def_member( "z", &mt::quatf::z )			
			;

		pybind::class_<Polygon>("Polygon")
			.def_convert( &ScriptMethod::Polygon_convert )
			.def_static("getPoints", &ScriptMethod::PolygonAdapter::s_getPoints)
			;

		pybind::class_<Viewport>("Viewport")
			.def( pybind::init<mt::vec2f,mt::vec2f>() )
			.def_convert( &ScriptMethod::Viewport_convert )
			.def_member( "begin", &Viewport::begin )
			.def_member( "end", &Viewport::end )
			;

		//pybind::class_<MovieFrame>("MovieFrame")
		//	.def_member( "anchorPoint", &MovieFrame::anchorPoint )
		//	.def_member( "position", &MovieFrame::position )
		//	.def_member( "scale", &MovieFrame::scale )
		//	.def_member( "angle", &MovieFrame::angle )
		//	.def_member( "opacity", &MovieFrame::opacity )
		//	;

		/*pybind::class_<Color>("Color")
		.def( pybind::init<float,float,float,float>() )
		.def( "set", &Color::set )
		.def( "getA", &Color::getA )
		.def( "getR", &Color::getR )
		.def( "getG", &Color::getG )
		.def( "getB", &Color::getB )
		;*/

		pybind::class_<ColourValue>("Color")
			.def( pybind::init<float, float, float, float>() )
			.def_convert( &ScriptMethod::color_convert )
			.def( "getA", &ColourValue::getA )
			.def( "getR", &ColourValue::getR )
			.def( "getG", &ColourValue::getG )
			.def( "getB", &ColourValue::getB )
			.def_property( "a", &ColourValue::getA, &ColourValue::setA )
			.def_property( "r", &ColourValue::getR, &ColourValue::setR )
			.def_property( "g", &ColourValue::getG, &ColourValue::setG )
			.def_property( "b", &ColourValue::getB, &ColourValue::setB )
			.def_repr( &ScriptMethod::color_repr )
			;

		//pybind::interface_<Join>("Join")
		//	.def( "getLeft", Join::getLeft )
		//	.def( "getRight", Join::getRight )
		//	;

		pybind::class_<Resolution>("Resolution")
			.def( pybind::init<std::size_t, std::size_t>() )
			.def_convert( &ScriptMethod::resolution_convert )
			.def( "setWidth", &Resolution::setWidth )
			.def( "setHeight", &Resolution::setHeight )
			.def( "getWidth", &Resolution::getWidth )
			.def( "getHeight", &Resolution::getHeight )
			.def( "getAspectRatio", &Resolution::getAspectRatio )
			;


		//pybind::class_<PhysicJoint2DInterface>("Joint2D", false)
		//	//.def( pybind::init<float,float>() )
		//	;

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
			;

		pybind::interface_<Transformation2D>("Transformation2D")
			.def( "setLocalPosition", &Transformation2D::setLocalPosition )
			.def( "getLocalPosition", &Transformation2D::getLocalPosition )
			.def( "getLocalDirection", &Transformation2D::getLocalDirection )
			.def( "setLocalDirection", &Transformation2D::setLocalDirection )
			.def( "setCoordinate", &Transformation2D::setCoordinate )			
			.def( "getCoordinate", &Transformation2D::getCoordinate )
			.def( "setOrigin", &Transformation2D::setOrigin )
			.def( "getOrigin", &Transformation2D::getOrigin )
			.def( "setScale", &Transformation2D::setScale )
			.def( "getScale", &Transformation2D::getScale )
			.def( "setAngle", &Transformation2D::setAngle )
			.def( "getAngle", &Transformation2D::getAngle )

			.def( "translate", &Transformation2D::translate )
			.def( "setFixedRotation", &Transformation2D::setFixedRotation )
			.def( "isFixedRotation", &Transformation2D::isFixedRotation )

			.def( "resetTransformation", &Transformation2D::resetTransformation )

			.def( "setRotate", &Transformation2D::setAngle ) //depricated
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
			;

		pybind::interface_<Animatable>("Animatable")
			.def( "play", &Animatable::play )
			.def( "stop", &Animatable::stop )
			.def( "isPlay", &Animatable::isPlay )
			.def( "setLoop", &Animatable::setLoop )
			.def( "getLoop", &Animatable::getLoop )
			;

		pybind::interface_<GlobalHandleAdapter>("GlobalHandleAdapter")
			.def( "enableGlobalMouseEvent", &GlobalHandleAdapter::enableGlobalMouseEvent )
			.def( "enableGlobalKeyEvent", &GlobalHandleAdapter::enableGlobalKeyEvent )				
			;

		pybind::interface_<Affectorable>("Affectorable")
			.def( "addAffector", &Affectorable::addAffector )
			;

		pybind::interface_<Node, pybind::bases<Scriptable,Identity,Transformation2D,Colorable,Resource,Renderable,GlobalHandleAdapter,Affectorable> >("Node", false)
			.def( "enable", &Node::enable )
			.def( "disable", &Node::disable )
			.def( "isEnable", &Node::isEnable )
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
			.def( "getWorldDirection", &Node::getWorldDirection )
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

		pybind::proxy_<Camera2D, pybind::bases<Node> >("Camera2D", false)
			.def( "setViewport", &Camera2D::setViewport )
			.def( "getViewport", &Camera2D::getViewport )
			.def( "setRenderport", &Camera2D::setRenderport )
			.def( "getRenderport", &Camera2D::getRenderport )
			.def( "setParallax", &Camera2D::setParallax )
			.def( "getParallax", &Camera2D::getParallax )
			.def( "setTargetNode", &Camera2D::setTargetNode )
			.def( "setTargetOffset", &Camera2D::setTargetOffset )
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
					.def( "interrupt", &ParticleEmitter::interrupt )
					.def( "setLeftBorder", &ParticleEmitter::setLeftBorder )
					.def( "setResource", &ParticleEmitter::setResource )
					.def( "setEmitter", &ParticleEmitter::setEmitter )
					.def( "setEmitterTranslateWithParticle", &ParticleEmitter::setEmitterTranslateWithParticle )
					.def( "setEmitterRelative", &ParticleEmitter::setEmitterRelative )
					.def( "setEmitterImage", &ParticleEmitter::setEmitterImage )
					.def( "changeEmitterPolygon", &ParticleEmitter::changeEmitterPolygon )
					.def( "setStartPosition", &ParticleEmitter::setStartPosition )
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
					.def( "getRenderViewport", &Layer2D::getRenderViewport )
					.def( "cameraToLocal", &Layer2D::cameraToLocal )
					;

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
					.def( "setFrame", &HotSpotImage::setFrame )
					.def( "getFrame", &HotSpotImage::getFrame )
					.def( "setAlphaTest", &HotSpotImage::setAlphaTest )
					.def( "getAlphaTest", &HotSpotImage::getAlphaTest )
					;


				pybind::proxy_<Sprite, pybind::bases<Node> >("Sprite", false)
					.def( "setImageIndex", &Sprite::setImageIndex )
					.def( "getImageIndex", &Sprite::getImageIndex )
					.def( "getImageCount", &Sprite::getImageCount )
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
						.def( "setAnimationResource", &Animation::setSequenceResource )
						.def( "getAnimationResource", &Animation::getSequenceResource )
						.def( "setAnimationFactor", &Animation::setAnimationFactor )
						.def( "getAnimationFactor", &Animation::getAnimationFactor )
						.def( "getFrameCount", &Animation::getFrameCount )
						.def( "getFrameDelay", &Animation::getFrameDelay )
						.def( "setCurrentFrame", &Animation::setCurrentFrame )
						.def( "getCurrentFrame", &Animation::getCurrentFrame )
						.def( "setTimming", &Animation::setTimming )
						.def( "getTimming", &Animation::getTimming )
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

				//pybind::proxy_<TilePolygon, pybind::bases<RigidBody2D> >("TilePolygon", false)
				//	;

				pybind::proxy_<Movie, pybind::bases<Node, Animatable> >("Movie", false)
					.def( "setFirstFrame", &Movie::setFirstFrame )
					.def( "setLastFrame", &Movie::setLastFrame )
					.def( "setReverse", &Movie::setReverse )
					.def( "getReverse", &Movie::getReverse )
					;

				pybind::proxy_<Video, pybind::bases<Node> >("Video", false)
					.def( "play", &Video::play )
					.def( "stop", &Video::stop )
					.def( "pause", &Video::pause )
					;

				pybind::proxy_<Window, pybind::bases<Node> >("Window", false)
					.def( "setClientSize", &Window::setClientSize )
					.def( "setClientSizeClip", &Window::setClientSizeClip )
					.def( "setClientSizeInTiles", &Window::setClientSizeInTiles )
					.def( "getClientSize", &Window::getClientSize )
					.def( "getWindowSize", &Window::getWindowSize )
					.def( "getTileSize", &Window::getTileSize )
					;

				//pybind::proxy_<Mesh_40_30, pybind::bases<Node> >("Mesh_40_30", false)
				//	.def( "setAmplitude", &Mesh_40_30::setAmplitude )
				//	.def( "setFrequency", &Mesh_40_30::setFrequency )
				//	;
			}		

			pybind::def( "setCurrentScene", &ScriptMethod::setCurrentScene );
			pybind::def( "getCurrentScene", &ScriptMethod::getCurrentScene );

			
			pybind::def( "hasScene", &ScriptMethod::s_hasScene );
			pybind::def( "createScene", &ScriptMethod::s_createScene );

			pybind::def( "setCamera2DPosition", &ScriptMethod::setCamera2DPosition );
			pybind::def( "getCamera2DPosition", &ScriptMethod::s_getCamera2DPosition );
			pybind::def( "setCamera2DDirection", &ScriptMethod::setCamera2DDirection );

			pybind::def( "createNode", &ScriptMethod::createNode );
			pybind::def( "createNodeFromBinary", &ScriptMethod::createNodeFromBinary );
			pybind::def( "destroyNode", &ScriptMethod::destroyNode );

			pybind::def( "timing", &ScriptMethod::timing );
			pybind::def( "timingRemove", &ScriptMethod::timingRemove );
			

			pybind::def( "schedule", &ScriptMethod::schedule );
			pybind::def( "scheduleRemove", &ScriptMethod::scheduleRemove );
			pybind::def( "scheduleRemoveAll", &ScriptMethod::scheduleRemoveAll );
			pybind::def( "scheduleFreeze", &ScriptMethod::s_scheduleFreeze );
			pybind::def( "scheduleFreezeAll", &ScriptMethod::s_scheduleFreezeAll );
			pybind::def( "scheduleResumeAll", &ScriptMethod::scheduleResumeAll );
			pybind::def( "scheduleIsFreeze", &ScriptMethod::s_scheduleIsFreeze );
			pybind::def( "scheduleTime", &ScriptMethod::s_scheduleTime );


			pybind::def( "getMouseX", &ScriptMethod::getMouseX ); //deprecated
			pybind::def( "getMouseY", &ScriptMethod::getMouseY ); //deprecated

			pybind::def( "getCursorPosition", &ScriptMethod::s_getCursorPosition );

			pybind::def( "setArrow", &ScriptMethod::s_setArrow );
			pybind::def( "getArrow", &ScriptMethod::s_getArrow );

			pybind::def( "setArrowLayer", &ScriptMethod::s_setArrowLayer );

			pybind::def( "directResourceCompile", &ScriptMethod::directResourceCompile );
			pybind::def( "directResourceRelease", &ScriptMethod::directResourceRelease );
			pybind::def( "directResourceFileCompile", &ScriptMethod::s_directResourceFileCompile );
			pybind::def( "deferredResourceFileCompile", &ScriptMethod::s_deferredResourceFileCompile );
			pybind::def( "directResourceFileRelease", &ScriptMethod::s_directResourceFileRelease );

			pybind::def( "quitApplication", &ScriptMethod::quitApplication );
			pybind::def( "createShot", &ScriptMethod::createShot );
			pybind::def( "setFullscreenMode", &ScriptMethod::setFullscreenMode );
			pybind::def( "getFullscreenMode", &ScriptMethod::s_getFullscreenMode );
			pybind::def( "addResourceListener", &ScriptMethod::addResourceListener );
			pybind::def( "renderOneFrame", &ScriptMethod::renderOneFrame );
			pybind::def( "writeImageToFile", &ScriptMethod::writeImageToFile );
			pybind::def( "createImageResource", &ScriptMethod::s_createImageResource );
			//pybind::def( "createFolder", &ScriptMethod::createFolder );
			//pybind::def( "deleteFolder", &ScriptMethod::deleteFolder );
			pybind::def( "minimizeWindow", &ScriptMethod::minimizeWindow );
			pybind::def( "setMouseBounded", &ScriptMethod::s_setMouseBounded );
			pybind::def( "getMouseBounded", &ScriptMethod::s_getMouseBounded );

			pybind::def( "getCurrentResolution", &ScriptMethod::s_getCurrentResolution );
			pybind::def( "getContentResolution", &ScriptMethod::s_getContentResolution );
			pybind::def( "getHotSpotImageSize", &ScriptMethod::s_getHotSpotImageSize );

			pybind::def( "setBlow", &ScriptMethod::setBlow );
			pybind::def( "getBlow", &ScriptMethod::getBlow );
			pybind::def( "setEnoughBlow", &ScriptMethod::setEnoughBlow );
			pybind::def( "setBlowCallback", &ScriptMethod::setBlowCallback );

			//pybind::def( "createDistanceJoint", &ScriptMethod::s_createDistanceJoint );
			//pybind::def( "createHingeJoint", &ScriptMethod::s_createHingeJoint );
			//pybind::def( "createMouseJoint", &ScriptMethod::s_createMouseJoint );
			//pybind::def( "destroyJoint", &ScriptMethod::s_destroyJoint );

			pybind::def( "isKeyDown", &ScriptMethod::s_isKeyDown );
			pybind::def( "isMouseButtonDown", &ScriptMethod::s_isMouseButtonDown );
			pybind::def( "isInViewport", &ScriptMethod::s_isInViewport );
			pybind::def( "getResourceCount", &ScriptMethod::s_getResourceCount );
			pybind::def( "enableTextureFiltering", &ScriptMethod::s_enableTextureFiltering );
			pybind::def( "isTextureFilteringEnabled", &ScriptMethod::s_isTextureFilteringEnabled );

			pybind::def( "existText", &ScriptMethod::s_existText );

			pybind::def( "pickHotspot", &ScriptMethod::s_pickHotspot );
			
			pybind::def( "blockInput", &ScriptMethod::s_blockInput );
			pybind::def( "setInputMouseButtonEventBlock", &ScriptMethod::s_setInputMouseButtonEventBlock );
			pybind::def( "getInputMouseButtonEventBlock", &ScriptMethod::s_getInputMouseButtonEventBlock );
			
			

			pybind::def( "getParam", &ScriptMethod::s_getParam );
			pybind::def( "addJoin", &ScriptMethod::s_addJoin );
			pybind::def( "removeJoin", &ScriptMethod::s_removeJoin );
			pybind::def( "isJoin", &ScriptMethod::s_isJoin );			
			pybind::def( "getJoins", &ScriptMethod::s_getJoins );

			pybind::def( "loadPlugin", &ScriptMethod::s_loadPlugin );

			//pybind::def( "loadText", &ScriptMethod::s_loadText );

			pybind::def( "getRenderCamera2D", &ScriptMethod::s_getRenderCamera2D );

			pybind::def( "testHotspot", &ScriptMethod::s_testHotspot);
			pybind::def( "polygon_wm", &ScriptMethod::s_polygon_wm);

			pybind::def( "createAffector", &ScriptMethod::createAffector );			
			pybind::def( "removeAllEvent", &ScriptMethod::removeAllEvent );

			pybind::def( "showKeyboard", &ScriptMethod::showKeyboard );			
			pybind::def( "hideKeyboard", &ScriptMethod::hideKeyboard );

			pybind::def( "hasResource", &ScriptMethod::hasResource );

			pybind::def( "removeCurrentScene", &ScriptMethod::removeCurrentScene );

			pybind::def( "setDefaultResourceFontName", &ScriptMethod::setDefaultResourceFontName );

			pybind::def( "addMouseMoveHandler", &ScriptMethod::s_addMouseMoveHandler );
			pybind::def( "removeMouseMoveHandler", &ScriptMethod::s_removeMouseMoveHandler );
			
			pybind::def( "addMouseButtonHandler", &ScriptMethod::s_addMouseButtonHandler );
			pybind::def( "removeMouseButtonHandler", &ScriptMethod::s_removeMouseButtonHandler );
		}
	}
}
