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

#	include "Player.h"
#	include "Application.h"

#	include "Amplifier.h"
#	include "Sprite.h"
#	include "Animation.h"
#	include "HotSpot.h"
#	include "Arrow.h"
#	include "TextField.h"
#	include "SoundEmitter.h"
#	include "Emitter.h"
#	include "Point.h"
#	include "Camera3d.h"
#	include "RigidBody3D.h"
#	include "CapsuleController.h"
#	include "Skeleton.h"

#	include "Layer2D.h"


#	include "Color.h"
#	include "C4AI.h"
#	include "ReversiAI.h"
#	include "CornersAI.h"

#	include "Camera3D.h"

#	include "SoundEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"

#	include "Camera3D.h"
#	include "Layer3D.h"

#	include "XmlEngine.h"


namespace Menge
{
	namespace ScriptMethod
	{
		static size_t schedule( float _timing, PyObject * _script )
		{
			return Holder<ScheduleManager>::hostage()
				->schedule( _timing, _script );
		}

		static void scheduleRemove( size_t _id )
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
		
		static int getMouseX()
		{
			return Holder<InputEngine>::hostage()
				->getMouseX();
		}

		static int getMouseY()
		{
			return Holder<InputEngine>::hostage()
				->getMouseY();
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
		
		static void setCamera3DPosition( float x, float y, float z )
		{
			Camera3D * camera = 
					Holder<Player>::hostage()
						->getRenderCamera3D();

			camera->setPosition(mt::vec3f(x,y,z));
		}

		static void setCamera2DDirection( float x, float y )
		{
			Scene * scene = getCurrentScene();

			struct ForeachRender
				: public NodeForeach
			{
				mt::vec2f dir;
				ForeachRender( const mt::vec2f& _dir )
					: dir(_dir)
				{}

				void apply( Node * children ) override
				{
					SceneNode2D * child = dynamic_cast<SceneNode2D*>(children);
					child->setLocalDirection(dir);
				}
			};

			scene->foreachChildren( ForeachRender(mt::vec2f(x,y)) );
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

			Scriptable * scriptable = node->getScriptable();
			PyObject * pyNode = scriptable->getScript();

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

		static PyObject * createShot( const std::string& _name, mt::vec2f _min,  mt::vec2f _max )
		{
			int rect[4] = { (int)_min.x , (int)_min.y, (int)_max.x, (int)_max.y };

			ResourceImageDynamic * resourceImage 
				= Holder<ResourceManager>::hostage()->getResourceT<ResourceImageDynamic>( _name );

			if( resourceImage == NULL )
			{
				resourceImage = new ResourceImageDynamic( _name );
				//FIXME
				RenderImageInterface * imageInterface
					= Holder<RenderEngine>::hostage()->createRenderTargetImage( _name.c_str(), rect[2] - rect[0], rect[3] - rect[1] );

				resourceImage->setRenderImage( imageInterface );

				Holder<ResourceManager>::hostage()->registerResource( resourceImage );
			}

			RenderImageInterface * image = resourceImage->getImage( 0 );

			//Holder<Application>::hostage()->update( 0.0f );
			Holder<Game>::hostage()->update(0.0f);
			Holder<Game>::hostage()->render();

			Holder<RenderEngine>::hostage()->render( image, rect );

			Sprite * nodeSprite = SceneManager::createNodeT<Sprite>("Sprite");

			nodeSprite->setImageResource( _name );

			nodeSprite->activate();

			if( nodeSprite == 0 )
			{
				return pybind::ret_none();
			}

			Scriptable * scriptable = nodeSprite->getScriptable();
			PyObject * pyNode = scriptable->getScript();

			if( pyNode == 0 )
			{
				delete nodeSprite;
				return pybind::ret_none();
			}

			return pyNode;		
		}

		static void setFullscreenMode( bool _fullscreen )
		{
			Holder<RenderEngine>::hostage()->setFullscreenMode( _fullscreen );
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
			Holder<Game>::hostage()->render();
			Holder<RenderEngine>::hostage()->render();
		}
		static void writeImageToFile( const std::string& _resource, int _frame, const std::string& _filename )
		{
			const RenderImageInterface* img = Holder<ResourceManager>::hostage()->getResourceT<ResourceImage>( _resource )->getImage( _frame );
			const_cast<RenderImageInterface*>(img)->writeToFile( _filename.c_str() );
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
			Layer2D* layer = Holder<Player>::hostage()->getCurrentScene()->getChildrenT<Layer2D>( _layerName, false );
			return layer->screenToLocal( _point );
		}
		static void minimizeWindow()
		{
			Holder<Application>::hostage()->minimizeWindow();
		}

	}

	SCRIPT_CLASS_WRAPPING( Node );
	SCRIPT_CLASS_WRAPPING( Scene );
	SCRIPT_CLASS_WRAPPING( HotSpot );
	SCRIPT_CLASS_WRAPPING( Sprite );
	SCRIPT_CLASS_WRAPPING( Animation );
	SCRIPT_CLASS_WRAPPING( Arrow );
	SCRIPT_CLASS_WRAPPING( TextField );
	SCRIPT_CLASS_WRAPPING( SoundEmitter );
	SCRIPT_CLASS_WRAPPING( Emitter );
	SCRIPT_CLASS_WRAPPING( Point );
	SCRIPT_CLASS_WRAPPING( SceneNode3D );
	SCRIPT_CLASS_WRAPPING( Camera3D );
	SCRIPT_CLASS_WRAPPING( RigidBody3D );
	SCRIPT_CLASS_WRAPPING( Layer3D );


	REGISTER_SCRIPT_CLASS( Menge, Node, Base )
	{
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

		pybind::interface_<Node>("Node", false)
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
			;

		pybind::interface_<Allocator2D>("Allocator2D", false)
			.def( "getLocalPosition", &Allocator2D::getLocalPosition )
			.def( "getLocalDirection", &Allocator2D::getLocalDirection )

			.def( "getWorldPosition", &Allocator2D::getWorldPosition )
			.def( "getWorldDirection", &Allocator2D::getWorldDirection )

			.def( "setLocalPosition", &Allocator2D::setLocalPosition )
			.def( "setLocalDirection", &Allocator2D::setLocalDirection )

			.def( "setWorldPosition", &Allocator2D::getWorldPosition )
			.def( "SetWorldDirection", &Allocator2D::getWorldDirection )
			.def( "setRotate", &Allocator2D::setRotate )
			;

		pybind::interface_<Skeleton>("Skeleton", false)
			.def( "enableAnimation", &Skeleton::enableAnimation )
			.def( "advanceAnimation", &Skeleton::advanceAnimation )
			.def( "advanceAllAnimations", &Skeleton::advanceAllAnimations )
			.def( "setAnimationWeight", &Skeleton::setAnimationWeight )
			.def( "attachEntityToBone", &Skeleton::attachEntityToBone )
			;

		pybind::interface_<NodeRenderable>("NodeRenderable", false)
			.def( "hide", &NodeRenderable::hide )
			;

		pybind::proxy_<SceneNode2D, pybind::bases<Node, Allocator2D, NodeRenderable>>("SceneNode2D", false)
				.def( "getScreenPosition", &SceneNode2D::getScreenPosition )
			;

		pybind::proxy_<SceneNode3D, pybind::bases<Node>>("SceneNode3D", false)
				.def( "getWorldOrient", &SceneNode3D::getWorldOrient )
				.def( "getWorldPosition", &SceneNode3D::getWorldPosition )
				.def( "getOrient", &SceneNode3D::getLocalOrient )
				.def( "getPosition", &SceneNode3D::getLocalPosition )
				.def( "setPosition", &SceneNode3D::setLocalPosition )
				.def( "setOrient", &SceneNode3D::setLocalOrient )
				.def( "setScale", &SceneNode3D::setScale )
				.def( "yaw", &SceneNode3D::yaw )
				.def( "pitch", &SceneNode3D::pitch )
				.def( "roll", &SceneNode3D::roll )
				.def( "translate", &SceneNode3D::translate )
				.def( "addChild", &SceneNode3D::addChild )
			;

		{
			pybind::proxy_<Layer3D, pybind::bases<SceneNode3D> >("Scene", false)
				.def( "addCamera", &Layer3D::addCamera )
				.def( "getCamera", &Layer3D::getCamera )
				.def( "addRigidBody", &Layer3D::addRigidBody )
				.def( "getRigidBody", &Layer3D::getRigidBody )
				.def( "addController", &Layer3D::addController )
				.def( "getController", &Layer3D::getController )		
				.def( "getNode", &Layer3D::getNode )
				;

			pybind::proxy_<RigidBody3D, pybind::bases<Node>>("RigidBody3D", false)
				.def( "applyForce", &RigidBody3D::applyForce )
				.def( "applyImpulse", &RigidBody3D::applyImpulse )
				.def( "applyAngularImpulse", &RigidBody3D::applyAngularImpulse )
				.def( "applyTorque", &RigidBody3D::applyTorque )
				.def( "setLinearVelocity", &RigidBody3D::setLinearVelocity )
				.def( "setAngularVelocity", &RigidBody3D::setAngularVelocity )
				.def( "setActive", &RigidBody3D::setActive )
				.def( "getPosition", &RigidBody3D::getPosition )
				.def( "getOrientation", &RigidBody3D::getOrientation )
				.def( "setPosition", &RigidBody3D::setPosition )
				.def( "setOrientation", &RigidBody3D::setOrientation )
				;

			pybind::proxy_<CapsuleController, pybind::bases<Node>>("CapsuleController", false)
				.def( "move", &CapsuleController::move )
				.def( "setPosition", &CapsuleController::setPosition )
				.def( "getFilteredPosition", &CapsuleController::getFilteredPosition )
				;

		pybind::proxy_<Camera3D, pybind::bases<SceneNode3D>>("Camera3D", false)
				.def( "setPosition", &Camera3D::setPosition )
				.def( "lookAt", &Camera3D::lookAt )
				.def( "yaw", &Camera3D::yaw )
				.def( "pitch", &Camera3D::pitch )
				.def( "roll", &Camera3D::roll )
			;

		
	/*	pybind::proxy_<Entity3d, pybind::bases<Node>>("Entity3d", false)
			.def( "play", &Entity3d::play )
		;
*/
		}

		{
			pybind::proxy_<Emitter, pybind::bases<SceneNode2D>>("Emitter", false)
				.def( "play", &Emitter::play )
				.def( "stop", &Emitter::stop )
				.def( "pause", &Emitter::pause )
				.def( "setLooped", &Emitter::setLooped )
				.def( "getLooped", &Emitter::getLooped )
				.def( "setLeftBorder", &Emitter::setLeftBorder )
				;

			pybind::proxy_<SoundEmitter, pybind::bases<SceneNode2D>>("SoundEmitter", false)
				.def( "play", &SoundEmitter::play )
				.def( "pause", &SoundEmitter::pause )
				.def( "stop", &SoundEmitter::stop )
				.def( "isPlaying", &SoundEmitter::isPlaying )
				.def( "setVolume", &SoundEmitter::setVolume )
				.def( "getVolume", &SoundEmitter::getVolume )
				.def( "setLooped", &SoundEmitter::setLooped )
				.def( "isLooping", &SoundEmitter::isLooping )
				.def( "setSoundListener", &SoundEmitter::setSoundListener )
				.def( "setSoundResource", &SoundEmitter::setSoundResource )
				;

			pybind::proxy_<TextField, pybind::bases<SceneNode2D>>("TextField", false)
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
				;

			pybind::proxy_<Arrow, pybind::bases<SceneNode2D>>("Arrow", false)
				.def( "setOffsetClick", &Arrow::setOffsetClick )
				.def( "getOffsetClick", &Arrow::getOffsetClick )
				.def( "addHotSpot", &Arrow::addHotSpot )
				.def( "getCurrentHotSpot", &Arrow::getCurrentHotSpot )
				;

			pybind::proxy_<Point, pybind::bases<SceneNode2D>>("Point", false)
				.def( "testHotSpot", &Point::testHotSpot )
				;


			pybind::proxy_<Scene, pybind::bases<Node> >("Scene", false)
				.def( "layerAppend", &Scene::layerAppend )
				.def( "getNode", &Scene::getNode )
				.def( "getLayerSize", &Scene::getLayerSize )
				.def( "setOffsetPosition", &Scene::setOffsetPosition )
				;

			pybind::proxy_<HotSpot, pybind::bases<SceneNode2D>>("HotSpot", false)
				.def( "setHotspotListener", &HotSpot::setHotspotListener )
				.def( "enableGlobalMouseEvent", &HotSpot::enableGlobalMouseEvent )
				.def( "addPoint", &HotSpot::addPoint )
				.def( "clearPoints", &HotSpot::clearPoints )
				.def( "pick", &HotSpot::pick )
				;

			pybind::proxy_<Sprite, pybind::bases<SceneNode2D>>("Sprite", false)
				.def( "setImageIndex", &Sprite::setImageIndex )
				.def( "getImageIndex", &Sprite::getImageIndex )
				.def( "setImageResource", &Sprite::setImageResource )
				.def( "getImageResource", &Sprite::getImageResource )
				.def( "setScale", &Sprite::setScale )
				.def( "getScale", &Sprite::getScale )
				.def( "setPercentVisibility", &Sprite::setPercentVisibility )
				.def( "setColor", &Sprite::setColor )
				.def( "getColor", &Sprite::getColor )
				.def( "setAlpha", &Sprite::setAlpha )
				.def( "colorTo", &Sprite::colorTo )
				.def( "alphaTo", &Sprite::alphaTo )
				.def( "flip", &Sprite::flip )
				.def( "setListener", &Sprite::setListener )
				;
			{
				pybind::proxy_<Animation, pybind::bases<Sprite>>("Animation", false)
					.def( "play", &Animation::play )
					.def( "stop", &Animation::stop )
					.def( "pause", &Animation::pause )
					.def( "setLooped", &Animation::setLooped )
					.def( "getLooped", &Animation::getLooped )					
					.def( "setAnimationListener", &Animation::setAnimationListener )
					.def( "setAnimationResource", &Animation::setAnimationResource )
					.def( "setAnimationFactor", &Animation::setAnimationFactor )
					.def( "getAnimationFactor", &Animation::getAnimationFactor )
					;
			}
		}		

		pybind::def( "setCurrentScene", &ScriptMethod::setCurrentScene );
		pybind::def( "getCurrentScene", &ScriptMethod::getCurrentScene );
		pybind::def( "setCamera2DPosition", &ScriptMethod::setCamera2DPosition );
		pybind::def( "setCamera2DDirection", &ScriptMethod::setCamera2DDirection );
				
		pybind::def( "setCamera3DPosition", &ScriptMethod::setCamera3DPosition );

		pybind::def( "createNodeFromXml", &ScriptMethod::createNodeFromXml );

		pybind::def( "schedule", &ScriptMethod::schedule );
		pybind::def( "scheduleRemove", &ScriptMethod::scheduleRemove );
		pybind::def( "scheduleRemoveAll", &ScriptMethod::scheduleRemoveAll );
		pybind::def( "scheduleStopAll", &ScriptMethod::scheduleStopAll );
		pybind::def( "scheduleResumeAll", &ScriptMethod::scheduleResumeAll );

		pybind::def( "getMouseX", &ScriptMethod::getMouseX );
		pybind::def( "getMouseY", &ScriptMethod::getMouseY );

		pybind::def( "getArrow", &ScriptMethod::getArrow );

		pybind::def( "directResourceCompile", &ScriptMethod::directResourceCompile );
		pybind::def( "directResourceRelease", &ScriptMethod::directResourceRelease );

		pybind::def( "quitApplication", &ScriptMethod::quitApplication );
		pybind::def( "createShot", &ScriptMethod::createShot );
		pybind::def( "setFullscreenMode", &ScriptMethod::setFullscreenMode );
		pybind::def( "addResourceListener", &ScriptMethod::addResourceListener );
		pybind::def( "removeResourceListener", &ScriptMethod::removeResourceListener );
		pybind::def( "renderOneFrame", &ScriptMethod::renderOneFrame );
		pybind::def( "writeImageToFile", &ScriptMethod::writeImageToFile );
		pybind::def( "createResourceFromXml", &ScriptMethod::createResourceFromXml );
		pybind::def( "createFolder", &ScriptMethod::createFolder );
		pybind::def( "deleteFolder", &ScriptMethod::deleteFolder );
		pybind::def( "screenToLocal", &ScriptMethod::screenToLocal );
		pybind::def( "minimizeWindow", &ScriptMethod::minimizeWindow );

	}
}