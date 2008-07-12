#	include "ScriptWrapper.h"

#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "InputEngine.h"
#	include "SceneManager.h"
#	include "FileEngine.h"
#	include "Scene.h"
#	include "Game.h"

#	include "ScriptEngine.h"
#	include "ScheduleManager.h"

#	include "ResourceManager.h"
#	include "ResourceImageDynamic.h"
#	include "ResourceImageDefault.h"

#	include "Player.h"
#	include "Application.h"

#	include "Amplifier.h"
#	include "Sprite.h"
#	include "Animation.h"
#	include "HotSpot.h"
#	include "Light2D.h"
#	include "ShadowCaster2D.h"
#	include "Arrow.h"
#	include "TextField.h"
#	include "SoundEmitter.h"
#	include "Emitter.h"
#	include "Point.h"
//#	include "Camera3d.h"
//#	include "RigidBody3D.h"
//#	include "CapsuleController.h"
#	include "Skeleton.h"
#	include "Camera2D.h"

#	include "Layer2D.h"

#	include "RigidBody2D.h"

#	include "TilePolygon.h"


#	include "Color.h"
#	include "C4AI.h"
#	include "ReversiAI.h"
#	include "CornersAI.h"

//#	include "DiscreteEntity.h"

#	include "SoundEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"

#	include "Identity.h"

//#	include "Layer3D.h"

//#	include "FFCamera.h"
#	include "SceneNode3D.h"
#	include "Camera3D.h"
#	include "RenderMesh.h"

#	include "XmlEngine.h"
#	include "Image.h"


namespace Menge
{
	namespace ScriptMethod
	{
		static unsigned int schedule( float _timing, PyObject * _script )
		{
			return Holder<ScheduleManager>::hostage()
				->schedule( _timing, _script );
		}

		static unsigned int s_scheduleTimer( float _timing, PyObject* _script )
		{
			return Holder<ScheduleManager>::hostage()->timerSchedule( _timing, _script );
		}

		static void scheduleRemove( unsigned int _id )
		{
			Holder<ScheduleManager>::hostage()
				->remove( _id );
		}

		static void scheduleRemoveAll()
		{
			Holder<ScheduleManager>::hostage()
				->removeAll();
		}	

		static void scheduleStopAll()
		{
			Holder<ScheduleManager>::hostage()
				->setUpdatable( false );
		}

		static void scheduleResumeAll()
		{
			Holder<ScheduleManager>::hostage()
				->setUpdatable( true );
		}

		static void s_scheduleFreeze( unsigned int _id, bool _freeze )
		{
			Holder<ScheduleManager>::hostage()
				->freeze( _id, _freeze );
		}
		
		static float getMouseX()
		{
			//return Holder<InputEngine>::hostage()
			//	->getMouseX();
			return Holder<Player>::hostage()->getArrow()->getLocalPosition().x;
		}

		static float getMouseY()
		{
			//return Holder<InputEngine>::hostage()
			//	->getMouseY();
			return Holder<Player>::hostage()->getArrow()->getLocalPosition().y;
		}

		static bool s_isKeyDown( int _key )
		{
			return Holder<InputEngine>::hostage()
				->isKeyDown( _key );
		}

		static void setCurrentScene( const std::string & _name, bool _destroyOld = false )
		{
			MENGE_LOG("set current scene '%s'\n", _name.c_str() );
			Holder<Player>::hostage()
					->setCurrentScene( _name, _destroyOld );
		}

		static Scene * getCurrentScene()
		{
			Scene * scene = Holder<Player>::hostage()
				->getCurrentScene();
			return scene;
		}

		static Arrow * getArrow()
		{
			Arrow * arrow = Holder<Player>::hostage()
				->getArrow();

			return arrow;
		}

		static void setCamera2DPosition( float x, float y )
		{
			Holder<Player>::hostage()
				->setCamera2DPosition( mt::vec2f(x, y) );
		}
		
		static void setCamera2DDirection( float x, float y )
		{
		/*	Scene * scene = getCurrentScene();

			struct ForeachRender
				: public NodeForeach
			{
				mt::vec2f dir;
				ForeachRender( const mt::vec2f& _dir )
					: dir(_dir)
				{}

				void apply( Node * children ) override
				{
					Node * child = dynamic_cast<Node*>(children);
					child->setLocalDirection(dir);
				}
			};

			scene->foreachChildren( ForeachRender(mt::vec2f(x,y)) );
			*/
		}

		static PyObject * createNodeFromXml( PyObject * _params  )
		{
			if( pybind::convert::is_string( _params ) == false )
			{
				return pybind::ret_none();
			}

			const char * xml_data = pybind::convert::to_string( _params );

			Node * node = SceneManager::createNodeFromXmlData( xml_data );

			if( node == 0 )
			{
				return pybind::ret_none();
			}

			PyObject * pyNode = node->getEmbedding();

			if( pyNode == 0 )
			{
				delete node;
				return pybind::ret_none();
			}

			return pyNode;
		}

		static void quitApplication()
		{
			Holder<Application>::hostage()->quit();
		}

		static bool directResourceCompile( const std::string & _nameResource )
		{
			bool result = Holder<ResourceManager>::hostage()
				->directResourceCompile( _nameResource );

			return result;
		}

		static void directResourceRelease( const std::string & _nameResource )
		{
			Holder<ResourceManager>::hostage()
				->directResourceRelease( _nameResource );
		}

		static void directResourceUnload( const std::string & _nameResource )
		{
			Holder<ResourceManager>::hostage()
				->directResourceUnload( _nameResource );
		}

		static PyObject * createShot( const std::string& _name, mt::vec2f _min,  mt::vec2f _max )
		{
			int rect[4] = { (int)_min.x , (int)_min.y, (int)_max.x, (int)_max.y };

			ResourceImageDynamic * resourceImage 
				= Holder<ResourceManager>::hostage()->getResourceT<ResourceImageDynamic>( _name );

			if( resourceImage == NULL )
			{
				ResourceFactoryParam param;
				param.name = _name;

				//TODO: Fix me. wtf i need to pass to category????
				param.category = "";

				resourceImage = new ResourceImageDynamic( param );
				//FIXME
				RenderImageInterface * imageInterface
					= Holder<RenderEngine>::hostage()->createImage( _name.c_str(), rect[2] - rect[0], rect[3] - rect[1] );
				resourceImage->setRenderImage( imageInterface );

				Holder<ResourceManager>::hostage()->registerResource( resourceImage );
			}

			RenderImageInterface * image = resourceImage->getImage( 0 );

			//Holder<Application>::hostage()->update( 0.0f );
			Holder<Game>::hostage()->update(0.0f);
			
			Holder<RenderEngine>::hostage()->beginScene();
		
			Holder<Game>::hostage()->render();

			Holder<RenderEngine>::hostage()->screenshot( image, rect );

			Holder<RenderEngine>::hostage()->endScene();

			//image->writeToFile( "bl.bmp" );

			Sprite * nodeSprite = SceneManager::createNodeT<Sprite>("Sprite");

			nodeSprite->setImageResource( _name );

			nodeSprite->activate();

			if( nodeSprite == 0 )
			{
				return pybind::ret_none();
			}

			PyObject * pyNode = nodeSprite->getEmbedding();

			if( pyNode == 0 )
			{
				delete nodeSprite;
				return pybind::ret_none();
			}

			return pyNode;		
		}

		static void setFullscreenMode( bool _fullscreen )
		{
			Holder<Application>::hostage()->setFullscreenMode( _fullscreen );
			//Holder<Application>::hostage()->setMouseBounded( _fullscreen );
			//Holder<RenderEngine>::hostage()->setFullscreenMode( _fullscreen );
		}

		static bool s_getFullscreenMode()
		{
			return Holder<RenderEngine>::hostage()->getFullscreenMode();
		}

		static void addResourceListener( PyObject* _listener )
		{
			Holder<ResourceManager>::hostage()->addListener( _listener );
		}
		static void removeResourceListener( PyObject* _listener )
		{
			Holder<ResourceManager>::hostage()->removeListener( _listener );
		}
		static void renderOneFrame()
		{
			Holder<RenderEngine>::hostage()->beginScene();
			Holder<Game>::hostage()->render();
			Holder<RenderEngine>::hostage()->endScene();
			Holder<RenderEngine>::hostage()->render();
		}
		static void writeImageToFile( const std::string& _resource, int _frame, const std::string& _filename )
		{
			RenderImageInterface* img = const_cast<RenderImageInterface*>( Holder<ResourceManager>::hostage()->getResourceT<ResourceImage>( _resource )->getImage( _frame ) );
			Image wImage;
			wImage.loadDynamicImage( img->lock(), img->getWidth(), img->getHeight(), 1, img->getPixelFormat() );
			wImage.save( Holder<FileEngine>::hostage()->getAppDataPath() + "\\" + _filename );
			img->unlock();
			//const_cast<RenderImageInterface*>(img)->writeToFile( _filename.c_str() );
		}
		static void setSoundEnabled( bool _enabled )
		{
			Holder<Application>::hostage()->setSoundEnabled( _enabled );
		}
		static void setParticlesEnabled( bool _enabled )
		{
			Holder<Application>::hostage()->setParticlesEnabled( _enabled );
		}
		static void createResourceFromXml( const std::string& _xml )
		{
			Holder<ResourceManager>::hostage()->createResourceFromXml( _xml );
		}

		static void s_createImageResource( const String& _resourceName, const String& _filename )
		{
			ResourceImageDefault* resImage = 
				static_cast<ResourceImageDefault*>
				( Holder<ResourceManager>::hostage()->createResource( _resourceName, "ResourceImageDefault" ) );
			resImage->addImagePath( _filename );
			Holder<ResourceManager>::hostage()->registerResource( resImage );
		}

		static bool createFolder( const std::string& _path )
		{
			return Holder<FileEngine>::hostage()->createFolder( _path );
		}
		static bool deleteFolder( const std::string& _path )
		{
			return Holder<FileEngine>::hostage()->deleteFolder( _path );
		}
		static mt::vec2f screenToLocal( const std::string& _layerName, const mt::vec2f& _point )
		{
			//Layer2D* layer = Holder<Player>::hostage()->getCurrentScene()->getChildrenT<Layer2D>( _layerName, false );
			//return layer->screenToLocal( _point );
			return mt::vec2f(0,0);
		}
		static void minimizeWindow()
		{
			Holder<Application>::hostage()->minimizeWindow();
		}
		static void s_setMouseBounded( bool _bounded )
		{
			Holder<Application>::hostage()->setMouseBounded( _bounded );
		}
		static bool setBlow( bool _active )
		{
			return Holder<SoundEngine>::hostage()
				->setBlow( _active );
		}
		static float getBlow()
		{
			return Holder<SoundEngine>::hostage()
				->getBlow();
		}
		static void setEnoughBlow( float _enoughBlow )
		{
			Holder<SoundEngine>::hostage()
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
			Holder<SoundEngine>::hostage()
				->setSulkCallback( new PySoundSulkCallback( _sulkcallback ) );
		}

		static PhysicJoint2DInterface* s_createDistanceJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offset1, const mt::vec2f& _offset2, bool _collideBodies )
		{
			return Holder<PhysicEngine2D>::hostage()->createDistanceJoint( _body1, _body2, _offset1, _offset2, _collideBodies );
		}

		static PhysicJoint2DInterface* s_createHingeJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offset1, const mt::vec2f& _limits, bool _collideBodies )
		{
			return Holder<PhysicEngine2D>::hostage()->createHingeJoint( _body1, _body2, _offset1, _limits, _collideBodies );
		}

		static void s_destroyJoint( PhysicJoint2DInterface* _joint )
		{
			return Holder<PhysicEngine2D>::hostage()->destroyJoint( _joint );
		}

		static void s_setCamera2DTarget( PyObject* _object )
		{
			Entity * entity = pybind::extract<Entity*>( _object);

			Holder<Player>::hostage()->getRenderCamera2D()->setTarget( (Node*)entity );
		}

		static void s_enableCamera2DTargetFollowing( bool _enable, float _force )
		{
			Holder<Player>::hostage()->getRenderCamera2D()->enableTargetFollowing( _enable, _force );
		}

		static void s_setCamera2DBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower )
		{
			Holder<Player>::hostage()->getRenderCamera2D()->setBounds( _leftUpper, _rightLower );
		}

		static void s_setCursorPosition( int _x, int _y )
		{
			Arrow* arrow = Holder<Player>::hostage()->getArrow();
			arrow->setLocalPosition( mt::vec2f( _x, _y ) + arrow->getOffsetClick() );
		}

		static bool s_isInViewport( const mt::vec2f _pos )
		{
			return Holder<RenderEngine>::hostage()->getRenderViewport().testPoint( _pos );
		}
	}

	static void classWrapping()
	{
		SCRIPT_CLASS_WRAPPING( Node );
		SCRIPT_CLASS_WRAPPING( Scene );
		SCRIPT_CLASS_WRAPPING( HotSpot );
		SCRIPT_CLASS_WRAPPING( Light2D );
		SCRIPT_CLASS_WRAPPING( ShadowCaster2D );
		SCRIPT_CLASS_WRAPPING( Sprite );
		SCRIPT_CLASS_WRAPPING( Animation );
		SCRIPT_CLASS_WRAPPING( Arrow );
		SCRIPT_CLASS_WRAPPING( TextField );
		SCRIPT_CLASS_WRAPPING( SoundEmitter );
		SCRIPT_CLASS_WRAPPING( Emitter );
		SCRIPT_CLASS_WRAPPING( Point );
		SCRIPT_CLASS_WRAPPING( TilePolygon );
		//SCRIPT_CLASS_WRAPPING( FFCamera3D );
		//SCRIPT_CLASS_WRAPPING( DiscreteEntity );
		//SCRIPT_CLASS_WRAPPING( RigidBody3D );
		//SCRIPT_CLASS_WRAPPING( Layer3D );
		SCRIPT_CLASS_WRAPPING( RigidBody2D );
		//SCRIPT_CLASS_WRAPPING( CapsuleController );
//		SCRIPT_CLASS_WRAPPING( SceneNode3D );
//		SCRIPT_CLASS_WRAPPING( Camera3D );
//		SCRIPT_CLASS_WRAPPING( RenderMesh );
	}

	//REGISTER_SCRIPT_CLASS( Menge, Node, Base )
	void ScriptWrapper::nodeWrap()
	{
		classWrapping();

		pybind::class_<mt::vec2f>("vec2f")
			.def( pybind::init<float,float>() )
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
			;

		pybind::class_<mt::quatf>("quatf")
			.def( pybind::init<float,float,float,float>() )
			;

		pybind::class_<Color>("Color")
			.def( pybind::init<float,float,float,float>() )
			.def( "set", &Color::set )
			.def( "getA", &Color::getA )
			.def( "getR", &Color::getR )
			.def( "getG", &Color::getG )
			.def( "getB", &Color::getB )
			;

		pybind::class_<PhysicJoint2DInterface>("Joint2D")
			//.def( pybind::init<float,float>() )
			;

		pybind::class_<Identity>("Identity")
			//.def( pybind::init<float,float>() )
		;


		pybind::class_<ReversiAI>("ReversiAI")
			.def( pybind::init<>() )
			.def( "setId", &ReversiAI::setId )
			.def( "setDepth", &ReversiAI::setDepth )
			.def( "setEndgameEmpty", &ReversiAI::setEndgameEmpty )
			.def( "markCell", &ReversiAI::markCell )
			.def( "autoMove", &ReversiAI::autoMove )
			.def( "getCurScore", &ReversiAI::getCurScore )
			.def( "getLeavesEvaluated", &ReversiAI::getLeavesEvaluated )
			.def( "getLegalMovesCount", &ReversiAI::getLegalMovesCount )
			;

		pybind::class_<CornersAI>("CornersAI")
			.def( pybind::init<>() )
			.def( "changeTurn", &CornersAI::changeTurn )
			.def( "makeMove", &CornersAI::makeMove )
			.def( "autoMove", &CornersAI::autoMove )
			.def( "getWinner", &CornersAI::getWinner )
			.def( "restartGame", &CornersAI::restartGame )
			;		
		
		pybind::class_<C4AI>("C4AI")
			.def( pybind::init<>() )
			.def( "newGame", &C4AI::newGame )
			.def( "makeMove", &C4AI::makeMove )
			.def( "autoMove", &C4AI::autoMove )
			.def( "endGame", &C4AI::endGame )
			.def( "reset", &C4AI::reset )
			;

		pybind::interface_<Allocator2D>("Allocator2D", false)
			.def( "getLocalPosition", &Allocator2D::getLocalPosition )
			.def( "getLocalDirection", &Allocator2D::getLocalDirection )

			.def( "getWorldPosition", &Allocator2D::getWorldPosition )
			.def( "getWorldDirection", &Allocator2D::getWorldDirection )

			.def( "setLocalPosition", &Allocator2D::setLocalPosition )
			.def( "setLocalDirection", &Allocator2D::setLocalDirection )

			.def( "setRotate", &Allocator2D::setRotate )
			;

		//Identity в bases? в некоторых местах без этого падало
	/*	pybind::interface_<Node, pybind::bases<Identity, Allocator2D>>("Node", false)
			.def( "activate", &Node::activate )
			.def( "deactivate", &Node::deactivate )
			.def( "isActivate", &Node::isActivate )
			.def( "compile", &Node::compile )
			.def( "release", &Node::release )
			.def( "destroy", &Node::destroy )
			.def( "isCompile", &Node::isCompile )
			.def( "enable", &Node::enable )
			.def( "disable", &Node::disable )
			.def( "isEnable", &Node::isEnable )
			.def( "setUpdatable", &Node::setUpdatable )
			.def( "isUpdatable", &Node::isUpdatable )
			.def( "setName", &Node::setName )
			.def( "getName", &Node::getName )
			.def( "addChildren", &Node::addChildren )
			.def( "removeChildren", &Node::removeChildren )
			.def( "getChildren", &Node::getChildren )
			.def( "updatable", &Node::updatable )
			.def( "update", &Node::update )
			.def( "setListener", &Node::setListener )
			;*/

		/*pybind::interface_<Skeleton>("Skeleton", false)
			.def( "attachEntityToBone", &Skeleton::attachEntityToBone )
			;*/

		/*pybind::class_<Camera3D>("Camera3D")
			.def( pybind::init<>() )
			.def( "setPosition", &Camera3D::setPosition )
			.def( "lookAt", &Camera3D::lookAt )
			//.def( "yaw", &Camera3D::yaw )
			.def( "pitch", &Camera3D::pitch )
			.def( "roll", &Camera3D::roll )
			.def( "getDirection", &Camera3D::getDirection )
			;*/

		
		/*pybind::class_<FFCamera3D>("FFCamera3D")
			.def( pybind::init<>() )
			.def( "update", &FFCamera3D::update )
			.def( "activate", &FFCamera3D::activate )
			.def( "forward", &FFCamera3D::forward )
			.def( "left", &FFCamera3D::left )
			.def( "setActor", &FFCamera3D::setActor )
			.def( "yaw", &FFCamera3D::yaw )
			.def( "pitch", &FFCamera3D::pitch )
			.def( "zoom", &FFCamera3D::zoom )
			;*/

	/*	pybind::interface_<NodeRenderable>("NodeRenderable", false)
			.def( "hide", &NodeRenderable::hide )
			;*/

	/*	pybind::proxy_<Node, pybind::bases<Node, Allocator2D, NodeRenderable>>("Node", false)
				.def( "getScreenPosition", &Node::getScreenPosition )
				.def( "getParent", &Node::getParent )
				//.def( "setListener", &Node::setListener )
				.def( "isHide", &Node::isHide )
				.def( "alphaTo", &Node::alphaTo )
				.def( "setAlpha", &Node::setAlpha )
			;*/

		pybind::interface_<Node, pybind::bases<Identity,Allocator2D> >("Node", false)
			.def( "activate", &Node::activate )
			.def( "deactivate", &Node::deactivate )
			.def( "isActivate", &Node::isActivate )
			.def( "compile", &Node::compile )
			.def( "release", &Node::release )
			.def( "destroy", &Node::destroy )
			.def( "isCompile", &Node::isCompile )
			.def( "enable", &Node::enable )
			.def( "disable", &Node::disable )
			.def( "isEnable", &Node::isEnable )
			.def( "setUpdatable", &Node::setUpdatable )
			.def( "isUpdatable", &Node::isUpdatable )
			.def( "setName", &Node::setName )
			.def( "getName", &Node::getName )
			.def( "addChildren", &Node::addChildren )
			.def( "removeChildren", &Node::removeChildren )
			.def( "getChildren", &Node::getChildren )
			.def( "updatable", &Node::updatable )
			.def( "update", &Node::update )
			.def( "getParent", &Node::getParent )
			.def( "setListener", &Node::setListener )
			.def( "hide", &Node::hide )
			.def( "isHide", &Node::isHide )
			.def( "getWorldPosition", &Node::getWorldPosition )
			.def( "getWorldDirection", &Node::getWorldDirection )
			//.def( "alphaTo", &Node::alphaTo )
			//.def( "setAlpha", &Node::setAlpha )
			.def( "getScreenPosition", &Node::getScreenPosition )
			.def( "getParent", &Node::getParent )
			;


		/*pybind::proxy_<SceneNode3D, pybind::bases<Node>>("SceneNode3D", false)
				.def( "getWorldOrient", &SceneNode3D::getWorldOrient )
				.def( "getWorldPosition", &SceneNode3D::getWorldPosition )
				.def( "getLocalOrient", &SceneNode3D::getLocalOrient )
				.def( "getPosition", &SceneNode3D::getLocalPosition )
				.def( "setPosition", &SceneNode3D::setLocalPosition )
				.def( "setOrient", &SceneNode3D::setLocalOrient )
				.def( "setScale", &SceneNode3D::setScale )
				.def( "yaw", &SceneNode3D::yaw )
				.def( "pitch", &SceneNode3D::pitch )
				.def( "roll", &SceneNode3D::roll )
				.def( "setFixedYawAxis", &SceneNode3D::setFixedYawAxis )
				.def( "translate", &SceneNode3D::translate )
				.def( "addChild", &SceneNode3D::addChild )
				.def( "setYawSpeed", &SceneNode3D::setYawSpeed )
				.def( "setYawLimits", &SceneNode3D::setYawLimits )
				.def( "getYaw", &SceneNode3D::getYaw )
				.def( "getPitch", &SceneNode3D::getPitch )
				.def( "setListener", &SceneNode3D::setListener )

				//.def( "getCamera", &SceneNode3D::getCamera )
			;

		{*/
			/*pybind::proxy_<Layer3D, pybind::bases<SceneNode3D> >("Layer3D", false)
				//.def( "addCamera", &Layer3D::addCamera )
				//.def( "getCamera", &Layer3D::getCamera )	
				.def( "addController", &Layer3D::addController )
				.def( "getController", &Layer3D::getController )		
				.def( "getNode", &Layer3D::getNode )
				;*/

			/*pybind::proxy_<RigidBody3D, pybind::bases<Node>>("RigidBody3D", false)
				.def( "applyForce", &RigidBody3D::applyForce )
				.def( "applyImpulse", &RigidBody3D::applyImpulse )
				.def( "applyAngularImpulse", &RigidBody3D::applyAngularImpulse )
				.def( "applyTorque", &RigidBody3D::applyTorque )
				.def( "setLinearVelocity", &RigidBody3D::setLinearVelocity )
				.def( "setAngularVelocity", &RigidBody3D::setAngularVelocity )
				.def( "setActive", &RigidBody3D::setActive )
				;

			pybind::proxy_<CapsuleController, pybind::bases<Node>>("CapsuleController", false)
				.def( "move", &CapsuleController::move )
				.def( "setPosition", &CapsuleController::setPosition )
				.def( "getFilteredPosition", &CapsuleController::getFilteredPosition )
				;

			pybind::proxy_<Camera3D, pybind::bases<SceneNode3D> >("Camera3D", false)
				.def( "setPosition", &Camera3D::setPosition )
				.def( "lookAt", &Camera3D::lookAt )
			//	.def( "yaw", &Camera3D::yaw )
			//	.def( "pitch", &Camera3D::pitch )
			//	.def( "roll", &Camera3D::roll )
				.def( "getDirection", &Camera3D::getDirection )
				;

			pybind::proxy_<DiscreteEntity, pybind::bases<SceneNode3D>>("DiscreteEntity", false)
				.def( "createRenderToTexture", &DiscreteEntity::createRenderToTexture )
				.def( "playAnimation", &DiscreteEntity::playAnimation )
				.def( "setMaterial", &DiscreteEntity::setMaterial )
				//.def( "playAnimation", &DiscreteEntity::playAnimation )
				;

				;*/

	/*	pybind::proxy_<SceneNode3D, pybind::bases<Node> >("SceneNode3D", false)
					.def( "yaw", &SceneNode3D::yaw )
					.def( "pitch", &SceneNode3D::pitch )
					.def( "getYaw", &SceneNode3D::getYaw )
					.def( "getPitch", &SceneNode3D::getPitch )
					.def( "yawTime", &SceneNode3D::yawTime )
					.def( "pitchTime", &SceneNode3D::pitchTime )
					.def( "setListener", &SceneNode3D::setListener )
			;
		{

				pybind::proxy_< RenderMesh, pybind::bases<SceneNode3D> >("RenderMesh", false)
					.def( "createRenderTarget", &RenderMesh::createRenderTarget )
					.def( "setMaterial", &RenderMesh::setMaterial )
					;

				pybind::proxy_<Camera3D, pybind::bases<SceneNode3D> >("Camera3D", false)
				//	.def( "setPosition", &Camera3D_::setPosition )
				//	.def( "lookAt", &Camera3D_::lookAt )
				//	.def( "yaw", &Camera3D::yaw )
				//	.def( "pitch", &Camera3D::pitch )
				//	.def( "roll", &Camera3D::roll )
				//	.def( "getDirection", &Camera3D_::getDirection )
					;
		}
*/
		{
			pybind::proxy_<Emitter, pybind::bases<Node>>("Emitter", false)
				.def( "play", &Emitter::play )
				.def( "stop", &Emitter::stop )
				.def( "pause", &Emitter::pause )
				.def( "setLooped", &Emitter::setLooped )
				.def( "getLooped", &Emitter::getLooped )
				.def( "setLeftBorder", &Emitter::setLeftBorder )
				;

			pybind::proxy_<SoundEmitter, pybind::bases<Node>>("SoundEmitter", false)
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

			pybind::proxy_<TextField, pybind::bases<Node>>("TextField", false)
				.def( "setText", &TextField::setText )
				.def( "setHeight", &TextField::setHeight )
				.def( "getText", &TextField::getText )
				.def( "getHeight", &TextField::getHeight )
				.def( "setColor", &TextField::setColor )
				.def( "getColor", &TextField::getColor )
				.def( "colorTo", &TextField::colorTo )
				.def( "alphaTo", &TextField::alphaTo )
				.def( "setAlpha", &TextField::setAlpha )
				.def( "setOutlineColor", &TextField::setOutlineColor )
				.def( "getOutlineColor", &TextField::getOutlineColor )
				.def( "getLength", &TextField::getLength )
				.def( "setMaxLen", &TextField::setMaxLen )
				//.def( "setListener", &TextField::setListener )
				.def( "getLineOffset", &TextField::getLineOffset )
				.def( "setLineOffset", &TextField::setLineOffset )
				.def( "setResource", &TextField::setResource )
				.def( "setOutlineResource", &TextField::setOutlineResource )
				.def( "getCenterAlign", &TextField::getCenterAlign )
				;

			pybind::proxy_<Arrow, pybind::bases<Node>>("Arrow", false)
				.def( "setOffsetClick", &Arrow::setOffsetClick )
				.def( "getOffsetClick", &Arrow::getOffsetClick )
				.def( "addHotSpot", &Arrow::addHotSpot )
				.def( "getCurrentHotSpot", &Arrow::getCurrentHotSpot )
				;

			pybind::proxy_<Point, pybind::bases<Node>>("Point", false)
				.def( "testHotSpot", &Point::testHotSpot )
				;


			pybind::proxy_<Scene, pybind::bases<Node> >("Scene", false)
				.def( "layerAppend", &Scene::layerAppend )
				.def( "getNode", &Scene::getNode )
				.def( "getLayerSize", &Scene::getLayerSize )
				.def( "setOffsetPosition", &Scene::setOffsetPosition )
				.def( "setRenderTarget", &Scene::setRenderTarget )
				;


			pybind::proxy_<HotSpot, pybind::bases<Node>>("HotSpot", false)
				.def( "enableGlobalMouseEvent", &HotSpot::enableGlobalMouseEvent )
				.def( "enableGlobalKeyEvent", &HotSpot::enableGlobalKeyEvent )				
				.def( "addPoint", &HotSpot::addPoint )
				.def( "clearPoints", &HotSpot::clearPoints )
				.def( "pick", &HotSpot::pick )
				;

			pybind::proxy_<Sprite, pybind::bases<Node>>("Sprite", false)
				.def( "setImageIndex", &Sprite::setImageIndex )
				.def( "getImageIndex", &Sprite::getImageIndex )
				.def( "setImageResource", &Sprite::setImageResource )
				.def( "getImageResource", &Sprite::getImageResource )
				.def( "getImageSize", &Sprite::getImageSize )
				.def( "setScale", &Sprite::setScale )
				.def( "getScale", &Sprite::getScale )
				.def( "setPercentVisibility", &Sprite::setPercentVisibility )
				.def( "setColor", &Sprite::setColor )
				.def( "getColor", &Sprite::getColor )
				.def( "setAlpha", &Sprite::setAlpha )
				.def( "colorTo", &Sprite::colorTo )
				.def( "alphaTo", &Sprite::alphaTo )
				.def( "flip", &Sprite::flip )
				//.def( "setListener", &Sprite::setListener )
				.def( "getCenterAlign", &Sprite::getCenterAlign )
				;
			{
				pybind::proxy_<Animation, pybind::bases<Sprite>>("Animation", false)
					.def( "play", &Animation::play )
					.def( "stop", &Animation::stop )
					.def( "pause", &Animation::pause )
					.def( "setLooped", &Animation::setLooped )
					.def( "getLooped", &Animation::getLooped )					
					.def( "setAnimationResource", &Animation::setAnimationResource )
					.def( "setAnimationFactor", &Animation::setAnimationFactor )
					.def( "getAnimationFactor", &Animation::getAnimationFactor )
					;
			}

			pybind::proxy_<RigidBody2D, pybind::bases<Node>>("RigidBody2D", false)
				//.def( "setListener", &RigidBody2D::setListener )
				.def( "getListener", &RigidBody2D::getListener )
				.def( "applyForce", &RigidBody2D::applyForce )
				.def( "applyImpulse", &RigidBody2D::applyImpulse )
				.def( "setAngle", &RigidBody2D::setAngle )
				.def( "setDirection", &RigidBody2D::setDirection )
				.def( "setPosition", &RigidBody2D::setPosition )
				.def( "applyConstantForce", &RigidBody2D::applyConstantForce )
				.def( "removeConstantForce", &RigidBody2D::removeConstantForce )
				.def( "setDirectionForce", &RigidBody2D::setDirectionForce )
				.def( "wakeUp", &RigidBody2D::wakeUp )
				.def( "getMass", &RigidBody2D::getMass )
				.def( "getLinearVelocity", &RigidBody2D::getLinearVelocity )
				.def( "setLinearVelocity", &RigidBody2D::setLinearVelocity )
			;

			
			pybind::proxy_<TilePolygon, pybind::bases<RigidBody2D>>("TilePolygon", false)
			;
		}		

		pybind::def( "setCurrentScene", &ScriptMethod::setCurrentScene );
		pybind::def( "getCurrentScene", &ScriptMethod::getCurrentScene );
		pybind::def( "setCamera2DPosition", &ScriptMethod::setCamera2DPosition );
		pybind::def( "setCamera2DDirection", &ScriptMethod::setCamera2DDirection );
		pybind::def( "setCamera2DTarget", &ScriptMethod::s_setCamera2DTarget );
		pybind::def( "enableCamera2DFollowing", &ScriptMethod::s_enableCamera2DTargetFollowing );
		pybind::def( "setCamera2DBounds", &ScriptMethod::s_setCamera2DBounds );
			
		pybind::def( "createNodeFromXml", &ScriptMethod::createNodeFromXml );

		pybind::def( "schedule", &ScriptMethod::schedule );
		pybind::def( "scheduleTimer", &ScriptMethod::s_scheduleTimer );
		pybind::def( "scheduleRemove", &ScriptMethod::scheduleRemove );
		pybind::def( "scheduleRemoveAll", &ScriptMethod::scheduleRemoveAll );
		pybind::def( "scheduleStopAll", &ScriptMethod::scheduleStopAll );
		pybind::def( "scheduleResumeAll", &ScriptMethod::scheduleResumeAll );
		pybind::def( "scheduleFreeze", &ScriptMethod::s_scheduleFreeze );

		pybind::def( "getMouseX", &ScriptMethod::getMouseX );
		pybind::def( "getMouseY", &ScriptMethod::getMouseY );
		pybind::def( "setCursorPosition", &ScriptMethod::s_setCursorPosition );

		pybind::def( "getArrow", &ScriptMethod::getArrow );

		pybind::def( "directResourceCompile", &ScriptMethod::directResourceCompile );
		pybind::def( "directResourceRelease", &ScriptMethod::directResourceRelease );
		pybind::def( "directResourceUnload", &ScriptMethod::directResourceUnload );

		pybind::def( "quitApplication", &ScriptMethod::quitApplication );
		pybind::def( "createShot", &ScriptMethod::createShot );
		pybind::def( "setFullscreenMode", &ScriptMethod::setFullscreenMode );
		pybind::def( "getFullscreenMode", &ScriptMethod::s_getFullscreenMode );
		pybind::def( "addResourceListener", &ScriptMethod::addResourceListener );
		pybind::def( "removeResourceListener", &ScriptMethod::removeResourceListener );
		pybind::def( "renderOneFrame", &ScriptMethod::renderOneFrame );
		pybind::def( "writeImageToFile", &ScriptMethod::writeImageToFile );
		pybind::def( "createResourceFromXml", &ScriptMethod::createResourceFromXml );
		pybind::def( "createImageResource", &ScriptMethod::s_createImageResource );
		pybind::def( "createFolder", &ScriptMethod::createFolder );
		pybind::def( "deleteFolder", &ScriptMethod::deleteFolder );
		pybind::def( "screenToLocal", &ScriptMethod::screenToLocal );
		pybind::def( "minimizeWindow", &ScriptMethod::minimizeWindow );
		pybind::def( "setMouseBounded", &ScriptMethod::s_setMouseBounded );

		pybind::def( "setBlow", &ScriptMethod::setBlow );
		pybind::def( "getBlow", &ScriptMethod::getBlow );
		pybind::def( "setEnoughBlow", &ScriptMethod::setEnoughBlow );
		pybind::def( "setBlowCallback", &ScriptMethod::setBlowCallback );

		pybind::def( "createDistanceJoint", &ScriptMethod::s_createDistanceJoint );
		pybind::def( "createHingeJoint", &ScriptMethod::s_createHingeJoint );
		pybind::def( "destroyJoint", &ScriptMethod::s_destroyJoint );

		pybind::def( "isKeyDown", &ScriptMethod::s_isKeyDown );
		pybind::def( "isInViewport", &ScriptMethod::s_isInViewport );
	}
}