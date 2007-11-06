#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "InputEngine.h"
#	include "SceneManager.h"
#	include "Scene.h"

#	include "ScriptEngine.h"
#	include "ScheduleManager.h"

#	include "Player.h"

#	include "Amplifier.h"
#	include "Sprite.h"
#	include "Animation.h"
#	include "HotSpot.h"
#	include "Arrow.h"
#	include "TextField.h"
#	include "SoundEmitter.h"
#	include "SoundEngine.h"


#	include "Camera3D.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	namespace ScriptMethod
	{
		static size_t schedule( size_t _timing, PyObject * _script )
		{
			return Holder<ScheduleManager>::hostage()
				->schedule( _timing, _script );
		}

		static void scheduleRemove( size_t _id )
		{
			Holder<ScheduleManager>::hostage()
				->remove( _id );
		}

		static void setVolume( float _vol )
		{
			Holder<SoundEngine>::hostage()->setVolume( _vol );
		}

		static void musicPlayList( const std::string & _list )
		{
			Holder<Amplifier>::hostage()->playList( _list );
		}

		static void musicSetVolume( float _vol )
		{
			Holder<Amplifier>::hostage()->setVolume( _vol );
		}

		static float musicGetVolume()
		{
			return Holder<Amplifier>::hostage()->getVolume();
		}

		static void musicStop( )
		{
			Holder<Amplifier>::hostage()->stop();
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

		static void setCurrentScene( const std::string & _name )
		{
			printf("set current scene '%s'\n", _name.c_str() );
			Holder<Player>::hostage()
					->setCurrentScene( _name );
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
	}

	SCRIPT_CLASS_WRAPPING( Node );
	SCRIPT_CLASS_WRAPPING( Scene );
	SCRIPT_CLASS_WRAPPING( HotSpot );
	SCRIPT_CLASS_WRAPPING( Sprite );
	SCRIPT_CLASS_WRAPPING( Animation );
	SCRIPT_CLASS_WRAPPING( Arrow );
	SCRIPT_CLASS_WRAPPING( TextField );
	SCRIPT_CLASS_WRAPPING( SoundEmitter );

//	SCRIPT_CLASS_WRAPPING( Amplifier );


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

		//pybind::interface_<Amplifier>("Amplifier", false)
		//	;


		pybind::interface_<Node>("Node", false)
			.def( "activate", &Node::activate )
			.def( "deactivate", &Node::deactivate )
			.def( "compile", &Node::compile )
			.def( "release", &Node::release )
			.def( "setName", &Node::setName )
			.def( "getName", &Node::getName )
			.def( "addChildren", &Node::addChildren )
			.def( "getChildren", &Node::getChildren )
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
		
		pybind::interface_<Renderable2D>("Renderable2D", false)
			.def( "hide", &Renderable2D::hide )
			;

		pybind::proxy_<SceneNode2D, pybind::bases<Node, Allocator2D, Renderable2D>>("SceneNode2D", false)
			;

		{
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
				.def( "setColor", &TextField::setColor )
				.def( "setHeight", &TextField::setHeight )
				;

			pybind::proxy_<Arrow, pybind::bases<SceneNode2D>>("Arrow", false)
				.def( "setOffsetClick", &Arrow::setOffsetClick )
				.def( "getOffsetClick", &Arrow::getOffsetClick )
				;

			pybind::proxy_<Scene, pybind::bases<Node> >("Scene", false)
				.def( "layerAppend", &Scene::layerAppend )
				.def( "getNode", &Scene::getNode )
				;

			pybind::proxy_<HotSpot, pybind::bases<SceneNode2D>>("HotSpot", false)
				.def( "setHotspotListener", &HotSpot::setHotspotListener )
				.def( "addPoint", &HotSpot::addPoint )
				.def( "clearPoints", &HotSpot::clearPoints )
				;

			pybind::proxy_<Sprite, pybind::bases<SceneNode2D>>("Sprite", false)
				.def( "setImageIndex", &Sprite::setImageIndex )
				.def( "getImageIndex", &Sprite::getImageIndex )
				.def( "setImageResource", &Sprite::setImageResource )
				.def( "getImageResource", &Sprite::getImageResource )
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
					;

			}
		}		

		pybind::def( "setCurrentScene", &ScriptMethod::setCurrentScene );
		pybind::def( "getCurrentScene", &ScriptMethod::getCurrentScene );
		pybind::def( "setCamera2DPosition", &ScriptMethod::setCamera2DPosition );
				
		pybind::def( "createNodeFromXml", &ScriptMethod::createNodeFromXml );

		pybind::def( "schedule", &ScriptMethod::schedule );
		pybind::def( "scheduleRemove", &ScriptMethod::scheduleRemove );

		pybind::def( "getMouseX", &ScriptMethod::getMouseX );
		pybind::def( "getMouseY", &ScriptMethod::getMouseY );

		pybind::def( "getArrow", &ScriptMethod::getArrow );

		pybind::def( "setVolume", &ScriptMethod::setVolume );

		pybind::def( "musicPlayList", &ScriptMethod::musicPlayList );
		pybind::def( "musicSetVolume", &ScriptMethod::musicSetVolume );
		pybind::def( "musicGetVolume", &ScriptMethod::musicGetVolume );
		pybind::def( "musicStop", &ScriptMethod::musicStop );

	}
}