
#	include "ScriptWrapper.h"

#	include "InputEngine.h"
#	include "NodeManager.h"
#	include "FileEngine.h"
#	include "Scene.h"
#	include "Game.h"

#	include "Consts.h"

#	include "ScriptEngine.h"
#	include "ScheduleManager.h"

#	include "TextManager.h"
#	include "ArrowManager.h"
#	include "SceneManager.h"
#	include "AccountManager.h"

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
#	include "Logger/Logger.h"
#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"

#	include "Identity.h"

#	include "Affector.h"

//#	include "Layer3D.h"

//#	include "FFCamera.h"
#	include "SceneNode3D.h"
#	include "Camera3D.h"
#	include "RenderMesh.h"

#	include "TaskManager.h"
#	include "TaskDeferredLoading.h"


#	include "Utils/Math/vec4.h"
#	include "Utils/Math/mat3.h"
#	include "Utils/Math/mat4.h"
#	include "Utils/Math/quat.h"
#	include "Utils/Math/clamp.h"

#	include <sstream>

namespace Menge
{
	namespace ScriptMethod
	{
		static void s_particleChangeEmitterImage( ParticleEmitter * _emitter, const ConstString & _resourceImageName )
		{
			ResourceImage * resourceImage = ResourceManager::get()
				->getResourceT<ResourceImage>(_resourceImageName);

			if( resourceImage == 0 )
			{
				return;
			}

			_emitter->changeEmitterImage( resourceImage );
		}

		static void s_blockInput( bool _value )
		{
			MousePickerSystem::get()
				->block(_value);
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
			return sm->time( _id );
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

		static bool s_isMouseDown( int _button )
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
			Scene * scene = getCurrentScene();
			
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

		static PyObject * createNode( const ConstString & _name, const ConstString & _type, const ConstString & _tag )
		{
			Node * node = NodeManager::get()
				->createNode( _name, _type, _tag );

			if( node == 0 )
			{
				return pybind::ret_none();
			}

			node->loaded();

			Game::get()
				->addHomeless( node );

			PyObject * embedding = node->getEmbed();

			if( embedding == 0 )
			{
				node->destroy();
				return pybind::ret_none();
			}

			return embedding;
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

			PyObject * embedding = node->getEmbed();

			if( embedding == 0 )
			{
				node->destroy();
				return pybind::ret_none();
			}

			return embedding;
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
			if( pybind::convert::is_string( _pycategory ) == false )
			{
				return;
			}

			ConstString category = pybind::extract<ConstString>( _pycategory );

			TVectorConstString resourceFiles;
			if( pybind::convert::is_string( _resourceFiles ) == true )
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
					if( pybind::convert::is_string( listItem ) == false )
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

			PyObject * embedding = node->getEmbed();

			if( embedding == 0 )
			{
				node->destroy();
				return pybind::ret_none();
			}

			return embedding;		
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

				String nameAnsi = Application::get()
					->utf8ToAnsi( Helper::to_str(_name) );

				ScriptEngine::get()
					->callFunction( m_eventLoaded, "(s)", nameAnsi.c_str() );
			}

			void onResourceUnLoaded( const ConstString& _name ) override
			{
				if( m_eventLoaded == 0 )
				{
					return;
				}

				String nameAnsi = Application::get()
					->utf8ToAnsi( Helper::to_str(_name) );

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
		static mt::vec2f screenToLocal( const ConstString& _layerName, const mt::vec2f& _point )
		{
			Scene * scene = Player::get()->getCurrentScene();

			Node * node = scene->findChildren( _layerName, false );

			Layer2D * layer = dynamic_cast<Layer2D *>(node);

			if( layer == 0 )
			{
				MENGE_LOG_ERROR( "Error: screenToLocal Scene '%s' not have Layer2D '%s'"
					, scene->getName().c_str()
					, _layerName.c_str()
					);

				return mt::vec2f::zero_v2;
			}

			return layer->screenToLocal( _point );
		}

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

		static PhysicJoint2DInterface* s_createDistanceJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offset1, const mt::vec2f& _offset2, bool _collideBodies )
		{
			return PhysicEngine2D::get()->createDistanceJoint( _body1, _body2, _offset1, _offset2, _collideBodies );
		}

		static PhysicJoint2DInterface* s_createHingeJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offset1, const mt::vec2f& _limits, bool _collideBodies )
		{
			return PhysicEngine2D::get()->createHingeJoint( _body1, _body2, _offset1, _limits, _collideBodies );
		}

		static PhysicJoint2DInterface* s_createMouseJoint( RigidBody2D* _body, int _x, int _y )
		{
			return PhysicEngine2D::get()->createMouseJoint( _body, _x, _y );
		}

		static void s_destroyJoint( PhysicJoint2DInterface* _joint )
		{
			return PhysicEngine2D::get()->destroyJoint( _joint );
		}

		static void s_setCamera2DTarget( PyObject* _object )
		{
			Entity * entity = pybind::extract_nt<Entity *>( _object);

			Player::get()
				->getRenderCamera2D()
				->setTarget( (Node*)entity );
		}

		static void s_enableCamera2DTargetFollowing( bool _enable, float _force )
		{
			Player::get()
				->getRenderCamera2D()
				->enableTargetFollowing( _enable, _force );
		}

		static void s_setCamera2DBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower )
		{
			Player::get()
				->getRenderCamera2D()
				->setBounds( _leftUpper, _rightLower );
		}

		static mt::vec2f s_getCursorPosition()
		{
			const Resolution& contRes = Game::get()
				->getContentResolution();

			Arrow * arrow = Player::get()
				->getArrow();

			mt::vec2f mp = arrow->getLocalPosition();

			mp.x = mt::clamp( 0.0f, mp.x, static_cast<float>( contRes.getWidth() ) );
			mp.y = mt::clamp( 0.0f, mp.y, static_cast<float>( contRes.getHeight() ) );

			return mp;
		}


		static void s_setCursorPosition( float _x, float _y )
		{
			Arrow* arrow = Player::get()
				->getArrow();

			arrow->setLocalPosition( mt::vec2f( _x, _y ) + arrow->getOffsetClick() );
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

		static PyObject * s_pickHotspot()
		{
			Arrow * arrow = Player::get()
				->getArrow();

			TVectorPickerTraps traps;

			MousePickerSystem::get()
				->pickTrap( arrow, traps );

			PyObject * pyret = pybind::list_new(0);

			for( TVectorPickerTraps::iterator
				it = traps.begin(),
				it_end = traps.end();
			it != it_end;
			++it )
			{
				PyObject * embedding = (*it)->getEmbed();

				pybind::list_appenditem( pyret, embedding );

				pybind::decref( embedding );
			}

			return pyret;
		}

		//////////////////////////////////////////////////////////////////////////
		class NodeGetChild
		{
		public:
			Node * getChild( const std::string & _name )
			{
				ConstString c_name(_name);

				return m_parent->findChildren( c_name, false );
			}

			void setParent( Node * _parent )
			{
				m_parent = _parent;
			}

		protected:
			Node * m_parent;
		};
		//////////////////////////////////////////////////////////////////////////
		static PyObject * s_getChild( Node * _node )
		{
			PyObject * py_child = pybind::list_new(0);

			const TListChild & child = _node->getChild();

			for( TListChild::const_iterator
				it = child.begin(),
				it_end = child.end();
			it != it_end;
			++it )
			{
				PyObject * py_node = it->getEmbed();
				pybind::list_appenditem( py_child, py_node );
				pybind::decref(py_node);
			}

			return py_child;
		}
		//////////////////////////////////////////////////////////////////////////
		static NodeGetChild * s_get_child( Node * _node )
		{
			static NodeGetChild * instance = 0;

			if( instance == 0 )
			{
				instance = new NodeGetChild;
			}

			instance->setParent( _node );

			return instance;
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_set_child( Node * _node, NodeGetChild * _obj )
		{

		}

		class AffectorManager
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			static void moveStop( Node * _node )
			{
				_node->stopAffectors( ETA_POSITION );
				_node->setLinearSpeed( mt::vec2f::zero_v2 );
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t moveTo( Node * _node, float _time, const mt::vec2f& _point, PyObject* _cb )
			{
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
			static void angleStop( Node * _node )
			{
				_node->stopAffectors( ETA_ANGLE );
				_node->setAngularSpeed(0.f);
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t angleTo( Node * _node, float _time, float _angle, PyObject* _cb )
			{
				angleStop( _node );

				Affector* affector =
					NodeAffectorCreator::newNodeAffectorInterpolateLinear(
					_cb, ETA_ANGLE, _node, &Node::setAngle
					, _node->getAngle(), _angle, _time
					, &fabsf 
					);				

				float invTime = 1.0f / _time;
				float angularSpeed = ( _angle - _node->getAngle() ) * invTime;

				_node->setAngularSpeed( angularSpeed );				
				std::size_t id = _node->addAffector( affector );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t accAngleTo( Node * _node, float _time, float _angle, PyObject* _cb )
			{
				float angularSpeed = _node->getAngularSpeed();

				angleStop( _node );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateQuadratic(
					_cb, ETA_ANGLE, _node, &Node::setAngle
					, _node->getAngle(), _angle, angularSpeed, _time
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
				ColourValue color = _node->getLocalColor();
				color.setA( _alpha );

				std::size_t id = colorTo( _node, _time, color, _cb );

				return id;
			}
			//////////////////////////////////////////////////////////////////////////
			static std::size_t setPercentVisibilityTo( Sprite * _sprite, float _time, const mt::vec2f& _percentX, const mt::vec2f& _percentY, PyObject* _cb )
			{
				_sprite->stopAffectors( ETA_VISIBILITY );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateLinear(
					_cb, ETA_VISIBILITY, _sprite, &Sprite::setPercentVisibilityVec4f
					, _sprite->getPercentVisibility(), mt::vec4f( _percentX, _percentY ), _time, 
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
			ss << "<" << Py_TYPE(_obj)->tp_name << " object at " << _obj << " value " << _v->x << ", " << _v->y << ">";
			return ss.str();
		}

		//////////////////////////////////////////////////////////////////////////
		std::string color_repr( PyObject * _obj, ColourValue * _v )
		{
			std::stringstream ss;
			ss << "<" << Py_TYPE(_obj)->tp_name << " object at " << _obj << " value " << _v->getA() << ", " << _v->getR() << ", " << _v->getG() << ", " << _v->getB() << ">";
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
				if( pybind::list_size( _obj ) != 4 )
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

		static PyObject * s_getHotspotPolygon( HotSpot * _hs )
		{
			PyObject * pyret = pybind::list_new(0);

			const mt::polygon & polygon = _hs->getPolygon();
			const mt::TVectorPoints & points = polygon.get_points();
			
			for( mt::TVectorPoints::const_iterator
				it = points.begin(),
				it_end = points.end();
			it != it_end;
			++it )
			{
				PyObject * embedding = pybind::ptr( (*it) );

				pybind::list_appenditem( pyret, embedding );

				pybind::decref( embedding );
			}

			return pyret;
		}

		static void s_setHotspotPolygon( HotSpot * _hs, PyObject * _polygon )
		{
			if( pybind::list_check( _polygon ) == false )
			{
				return;
			}

			mt::polygon p;

			std::size_t size = pybind::list_size( _polygon );

			for( std::size_t i = 0; i != size; ++i )
			{
				PyObject * py_point = pybind::list_getitem(_polygon, i);

				mt::vec2f point = pybind::extract<mt::vec2f>(py_point);

				p.add_point( point );
			}

			_hs->setPolygon( p );
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

				PyObject * py_node = it->getEmbed();
				pybind::list_appenditem( py_filter, py_node );
				pybind::decref( py_node );
			}

			return py_filter;			
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
		SCRIPT_CLASS_WRAPPING( TilePolygon );
		SCRIPT_CLASS_WRAPPING( Video );
		//SCRIPT_CLASS_WRAPPING( FFCamera3D );
		//SCRIPT_CLASS_WRAPPING( DiscreteEntity );
		//SCRIPT_CLASS_WRAPPING( RigidBody3D );
		//SCRIPT_CLASS_WRAPPING( Layer3D );
		SCRIPT_CLASS_WRAPPING( RigidBody2D );
		//SCRIPT_CLASS_WRAPPING( CapsuleController );
		//SCRIPT_CLASS_WRAPPING( SceneNode3D );
		//SCRIPT_CLASS_WRAPPING( Camera3D );
		//SCRIPT_CLASS_WRAPPING( RenderMesh );
		SCRIPT_CLASS_WRAPPING( Window );
		SCRIPT_CLASS_WRAPPING( HotSpotImage );
		//SCRIPT_CLASS_WRAPPING( Mesh_40_30 );
		//SCRIPT_CLASS_WRAPPING( Camera2D );
		SCRIPT_CLASS_WRAPPING( Layer2DTexture );
	}

	struct extract_const_string_type
		: public pybind::type_cast_result<ConstString>
	{
		ConstString apply( PyObject * _obj ) override
		{
			m_valid = false;

			if( PyString_Check( _obj ) )
			{
				m_valid = true;

				const char * ch_buff = PyString_AsString(_obj);
				Py_ssize_t ch_size = PyString_Size(_obj);

				return ConstString( ch_buff, ch_size );
			}
			else if( PyUnicode_Check( _obj ) )
			{
				m_valid = true;
				PyObject* strObj = PyUnicode_AsUTF8String( _obj );

				const char * ch_buff = PyString_AsString(strObj);
				Py_ssize_t ch_size = PyString_Size(strObj);

				return ConstString( ch_buff, ch_size );
			}
			else if( _obj == Py_None )
			{
				m_valid = true;

				return Consts::get()->c_builtin_empty;
			}

			return Helper::to_none();
		}

		PyObject * wrap( ConstString _value ) override
		{
			return PyString_FromStringAndSize( _value.c_str(), _value.size() );
		}
	};

	static extract_const_string_type * st = 0; 

	void ScriptWrapper::finalize()
	{
		delete st;
	}

	//REGISTER_SCRIPT_CLASS( Menge, Node, Base )
	void ScriptWrapper::nodeWrap()
	{
		classWrapping();

		st = new extract_const_string_type();

		pybind::class_<ScriptMethod::NodeGetChild>( "NodeGetChild" )
			.def_getattro( &ScriptMethod::NodeGetChild::getChild )
			.def_mapping( &ScriptMethod::NodeGetChild::getChild )
			;

		pybind::class_<mt::vec2f>("vec2f")
			.def( pybind::init<float,float>() )
			.def_convert( &ScriptMethod::vec2f_convert )
			.def_member( "x", &mt::vec2f::x )
			.def_member( "y", &mt::vec2f::y )
			.def_repr( &ScriptMethod::vec2f_repr )
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

		pybind::class_<mt::polygon>("polygon")
			.def( "add_point", &mt::polygon::add_point )
			;

		pybind::class_<Viewport>("Viewport")
			.def( pybind::init<mt::vec2f,mt::vec2f>() )
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

		pybind::class_<Resolution>("Resolution")
			.def( pybind::init<std::size_t, std::size_t>() )
			.def_convert( &ScriptMethod::resolution_convert )
			.def( "setWidth", &Resolution::setWidth )
			.def( "setHeight", &Resolution::setHeight )
			.def( "getWidth", &Resolution::getWidth )
			.def( "getHeight", &Resolution::getHeight )
			.def( "getAspectRatio", &Resolution::getAspectRatio )
			;


		pybind::class_<PhysicJoint2DInterface>("Joint2D", false)
			//.def( pybind::init<float,float>() )
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
			.def( "getOrigin", &Transformation2D::getOrigin )
			.def( "setOrigin", &Transformation2D::setOrigin )
			.def( "getScale", &Transformation2D::getScale )
			.def( "setScale", &Transformation2D::setScale )
			.def( "getAngle", &Transformation2D::getAngle )
			.def( "setRotate", &Transformation2D::setAngle ) //depricated
			.def( "setAngle", &Transformation2D::setAngle )
			.def( "translate", &Transformation2D::translate )

			.def_property( "position", &Transformation2D::getLocalPosition, &Transformation2D::setLocalPosition )
			.def_property( "direction", &Transformation2D::getLocalDirection, &Transformation2D::setLocalDirection )
			.def_property( "scale", &Transformation2D::getScale, &Transformation2D::setScale )
			.def_property( "angle", &Transformation2D::getAngle, &Transformation2D::setAngle )
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
			.def_property( "color", &Colorable::getLocalColor, &Colorable::setLocalColor )
			.def_property( "alpha", &Colorable::getLocalAlpha, &Colorable::setLocalAlpha )
			;

		pybind::interface_<GlobalHandleAdapter>("GlobalHandleAdapter")
			.def( "enableGlobalMouseEvent", &GlobalHandleAdapter::enableGlobalMouseEvent )
			.def( "enableGlobalKeyEvent", &GlobalHandleAdapter::enableGlobalKeyEvent )				
			;

		pybind::interface_<Node, pybind::bases<Scriptable,Identity,Transformation2D,Colorable,Resource,Renderable,GlobalHandleAdapter> >("Node", false)
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
			.def( "removeFromParent", &Node::removeFromParent )
			.def_static( "getChild", &ScriptMethod::s_getChild )
			.def( "findChildren", &Node::findChildren )
			.def( "findTag", &Node::findTag )
			.def_static( "filterTag", &ScriptMethod::s_filterTag )
			.def( "isChildren", &Node::isChildren )
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
			.def( "getScreenPosition", &Node::getScreenPosition )

			.def( "getWorldColor", &Node::getWorldColor )

			.def_static( "colorTo", &ScriptMethod::AffectorManager::colorTo )
			.def_static( "alphaTo", &ScriptMethod::AffectorManager::alphaTo )
			.def_static( "colorStop", &ScriptMethod::AffectorManager::colorStop )

			.def_static( "moveTo", &ScriptMethod::AffectorManager::moveTo )
			.def_static( "moveStop", &ScriptMethod::AffectorManager::moveStop )

			.def_static( "angleTo", &ScriptMethod::AffectorManager::angleTo )
			.def_static( "angleStop", &ScriptMethod::AffectorManager::angleStop )
			.def_static( "scaleTo", &ScriptMethod::AffectorManager::scaleTo )
			.def_static( "scaleStop", &ScriptMethod::AffectorManager::scaleStop )

			.def_static( "accMoveTo", &ScriptMethod::AffectorManager::accMoveTo )
			.def_static( "accAngleTo", &ScriptMethod::AffectorManager::accAngleTo )

			.def_property_static( "child", &ScriptMethod::s_get_child, &ScriptMethod::s_set_child )
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
				pybind::proxy_<ParticleEmitter, pybind::bases<Node> >("ParticleEmitter", false)
					.def( "play", &ParticleEmitter::play )
					.def( "playFromPosition", &ParticleEmitter::playFromPosition )
					.def( "stop", &ParticleEmitter::stop )
					.def( "pause", &ParticleEmitter::pause )
					.def( "restart", &ParticleEmitter::restart )
					.def( "setLooped", &ParticleEmitter::setLooped )
					.def( "getLooped", &ParticleEmitter::getLooped )
					.def( "setAutoPlay", &ParticleEmitter::setAutoPlay )
					.def( "getAutoPlay", &ParticleEmitter::getAutoPlay )				
					.def( "setLeftBorder", &ParticleEmitter::setLeftBorder )
					.def( "setResource", &ParticleEmitter::setResource )
					.def( "setEmitter", &ParticleEmitter::setEmitter )
					.def( "setEmitterRelative", &ParticleEmitter::setEmitterRelative )
					.def_static( "changeEmitterImage", &ScriptMethod::s_particleChangeEmitterImage )
					.def( "setStartPosition", &ParticleEmitter::setStartPosition )
					;

				pybind::proxy_<SoundEmitter, pybind::bases<Node> >("SoundEmitter", false)
					.def( "play", &SoundEmitter::play )
					.def( "pause", &SoundEmitter::pause )
					.def( "stop", &SoundEmitter::stop )
					.def( "isPlaying", &SoundEmitter::isPlaying )
					.def( "setVolume", &SoundEmitter::setVolume )
					.def( "getVolume", &SoundEmitter::getVolume )
					.def( "setLooped", &SoundEmitter::setLooped )
					.def( "isLooping", &SoundEmitter::isLooping )
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
					.def( "getCenterAlign", &TextField::getCenterAlign )
					.def( "setCenterAlign", &TextField::setCenterAlign )
					.def( "getRightAlign", &TextField::getRightAlign )
					.def( "setRightAlign", &TextField::setRightAlign )
					.def( "getLeftAlign", &TextField::getLeftAlign )
					.def( "setLeftAlign", &TextField::setLeftAlign )
					.def( "getCharOffset", &TextField::getCharOffset )
					.def( "setCharOffset", &TextField::setCharOffset )
					.def( "setTextByKey", &TextField::setTextByKey )
					.def( "getTextKey", &TextField::getTextKey )
					;

				pybind::proxy_<Point, pybind::bases<Node> >("Point", false)
					.def( "testHotSpot", &Point::testHotSpot )
					;

				pybind::interface_<Layer, pybind::bases<Node> >("Layer", false)
					.def( "getSize", &Layer::getSize )
					;

				pybind::proxy_<Layer2D, pybind::bases<Layer> >("Layer2D", false)
					.def( "setParallaxFactor", &Layer2D::setParallaxFactor )
					.def( "getParallaxFactor", &Layer2D::getParallaxFactor )
					.def( "setRenderViewport", &Layer2D::setRenderViewport )
					.def( "getRenderViewport", &Layer2D::getRenderViewport )
					.def( "screenToLocal", &Layer2D::screenToLocal )
					;

				pybind::proxy_<Layer2DTexture, pybind::bases<Layer2D> >("Layer2DTexture", false)
					.def( "setCameraOffset", &Layer2DTexture::setCameraOffset )
					.def( "getCameraOffset", &Layer2DTexture::getCameraOffset )
					.def( "setRenderTargetName", &Layer2DTexture::setRenderTargetName )
					.def( "getRenderTargetName", &Layer2DTexture::getRenderTargetName )
					;

				pybind::proxy_<HotSpot, pybind::bases<Node> >("HotSpot", false)
					.def( "addPoint", &HotSpot::addPoint )
					.def( "testPoint", &HotSpot::testPoint )
					.def( "clearPoints", &HotSpot::clearPoints )
					.def_static( "getPolygon", &ScriptMethod::s_getHotspotPolygon )
					.def_static( "setPolygon", &ScriptMethod::s_setHotspotPolygon )
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
					//.def( "setScale", &Sprite::setScale )
					//.def( "getScale", &Sprite::getScale )
					.def( "setPercentVisibility", &Sprite::setPercentVisibility )
					.def_static( "setPercentVisibilityTo", &ScriptMethod::AffectorManager::setPercentVisibilityTo )
					.def_static( "setPercentVisibilityStop", &ScriptMethod::AffectorManager::setPercentVisibilityStop )
					.def( "setFlipX", &Sprite::setFlipX )
					.def( "setFlipY", &Sprite::setFlipY )
					.def( "getFlipX", &Sprite::getFlipX )
					.def( "getFlipY", &Sprite::getFlipY )
					.def( "getCenterAlign", &Sprite::getCenterAlign )
					.def( "setCenterAlign", &Sprite::setCenterAlign )
					.def( "setImageAlpha", &Sprite::setImageAlpha )
					.def( "setImageAlphaIndex", &Sprite::setImageAlphaIndex )
					.def( "disableTextureColor", &Sprite::disableTextureColor )
					.def( "setTextureMatrixOffset", &Sprite::setTextureMatrixOffset )
					
					;
				{
					pybind::proxy_<Animation, pybind::bases<Sprite> >("Animation", false)
						.def( "play", &Animation::play )
						.def( "stop", &Animation::stop )
						.def( "pause", &Animation::pause )
						.def( "resume", &Animation::resume )

						.def( "setLooped", &Animation::setLooped )
						.def( "getLooped", &Animation::getLooped )					
						.def( "setAutoPlay", &Animation::setAutoPlay )
						.def( "getAutoPlay", &Animation::getAutoPlay )					
						.def( "setAnimationResource", &Animation::setAnimationResource )
						.def( "setAnimationFactor", &Animation::setAnimationFactor )
						.def( "getAnimationFactor", &Animation::getAnimationFactor )
						.def( "getCurrentFrame", &Animation::getCurrentFrame )
						.def( "getFrameCount", &Animation::getFrameCount )
						.def( "setCurrentFrame", &Animation::setCurrentFrame )
						;
				}

				pybind::proxy_<RigidBody2D, pybind::bases<Node> >("RigidBody2D", false)
					.def( "applyForce", &RigidBody2D::applyForce )
					.def( "applyImpulse", &RigidBody2D::applyImpulse )
					.def( "applyConstantForce", &RigidBody2D::applyConstantForce )
					.def( "removeConstantForce", &RigidBody2D::removeConstantForce )
					.def( "setDirectionForce", &RigidBody2D::setDirectionForce )
					.def( "wakeUp", &RigidBody2D::wakeUp )
					.def( "getMass", &RigidBody2D::getMass )
					.def( "getLinearVelocity", &RigidBody2D::getLinearVelocity )
					.def( "setLinearVelocity", &RigidBody2D::setLinearVelocity )
					.def( "unsetLinearVelocity", &RigidBody2D::unsetLinearVelocity )
					.def( "setCollisionMask", &RigidBody2D::setCollisionMask )
					.def( "enableStabilization", &RigidBody2D::enableStabilization )
					;

				pybind::proxy_<TilePolygon, pybind::bases<RigidBody2D> >("TilePolygon", false)
					;

				pybind::proxy_<Movie, pybind::bases<Node> >("Movie", false)
					.def( "play", &Movie::play )
					.def( "stop", &Movie::stop )
					.def( "setAutoPlay", &Movie::setAutoPlay )
					.def( "setLoop", &Movie::setLoop )
					.def( "setComplete", &Movie::setComplete )
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

			pybind::def( "createScene", &ScriptMethod::s_createScene );

			pybind::def( "setCamera2DPosition", &ScriptMethod::setCamera2DPosition );
			pybind::def( "getCamera2DPosition", &ScriptMethod::s_getCamera2DPosition );
			pybind::def( "setCamera2DDirection", &ScriptMethod::setCamera2DDirection );
			pybind::def( "setCamera2DTarget", &ScriptMethod::s_setCamera2DTarget );
			pybind::def( "enableCamera2DFollowing", &ScriptMethod::s_enableCamera2DTargetFollowing );
			pybind::def( "setCamera2DBounds", &ScriptMethod::s_setCamera2DBounds );

			pybind::def( "createNode", &ScriptMethod::createNode );
			pybind::def( "createNodeFromBinary", &ScriptMethod::createNodeFromBinary );
			pybind::def( "destroyNode", &ScriptMethod::destroyNode );

			pybind::def( "schedule", &ScriptMethod::schedule );
			pybind::def( "scheduleRemove", &ScriptMethod::scheduleRemove );
			pybind::def( "scheduleRemoveAll", &ScriptMethod::scheduleRemoveAll );
			pybind::def( "scheduleFreeze", &ScriptMethod::s_scheduleFreeze );
			pybind::def( "scheduleFreezeAll", &ScriptMethod::s_scheduleFreezeAll );
			pybind::def( "scheduleResumeAll", &ScriptMethod::scheduleResumeAll );
			pybind::def( "scheduleIsFreeze", &ScriptMethod::s_scheduleIsFreeze);
			pybind::def( "scheduleTime", &ScriptMethod::s_scheduleTime );

			pybind::def( "getMouseX", &ScriptMethod::getMouseX ); //deprecated
			pybind::def( "getMouseY", &ScriptMethod::getMouseY ); //deprecated

			pybind::def( "getCursorPosition", &ScriptMethod::s_getCursorPosition );
			pybind::def( "setCursorPosition", &ScriptMethod::s_setCursorPosition );

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
			pybind::def( "screenToLocal", &ScriptMethod::screenToLocal );
			pybind::def( "minimizeWindow", &ScriptMethod::minimizeWindow );
			pybind::def( "setMouseBounded", &ScriptMethod::s_setMouseBounded );
			pybind::def( "getMouseBounded", &ScriptMethod::s_getMouseBounded );

			pybind::def( "getCurrentResolution", &ScriptMethod::s_getCurrentResolution );
			pybind::def( "getHotSpotImageSize", &ScriptMethod::s_getHotSpotImageSize );

			pybind::def( "setBlow", &ScriptMethod::setBlow );
			pybind::def( "getBlow", &ScriptMethod::getBlow );
			pybind::def( "setEnoughBlow", &ScriptMethod::setEnoughBlow );
			pybind::def( "setBlowCallback", &ScriptMethod::setBlowCallback );

			pybind::def( "createDistanceJoint", &ScriptMethod::s_createDistanceJoint );
			pybind::def( "createHingeJoint", &ScriptMethod::s_createHingeJoint );
			pybind::def( "createMouseJoint", &ScriptMethod::s_createMouseJoint );
			pybind::def( "destroyJoint", &ScriptMethod::s_destroyJoint );

			pybind::def( "isKeyDown", &ScriptMethod::s_isKeyDown );
			pybind::def( "isMouseDown", &ScriptMethod::s_isMouseDown );
			pybind::def( "isInViewport", &ScriptMethod::s_isInViewport );
			pybind::def( "getResourceCount", &ScriptMethod::s_getResourceCount );
			pybind::def( "enableTextureFiltering", &ScriptMethod::s_enableTextureFiltering );
			pybind::def( "isTextureFilteringEnabled", &ScriptMethod::s_isTextureFilteringEnabled );

			pybind::def( "existText", &ScriptMethod::s_existText );

			pybind::def( "pickHotspot", &ScriptMethod::s_pickHotspot );
			pybind::def( "blockInput", &ScriptMethod::s_blockInput );
		}
	}
}
