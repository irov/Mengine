#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "InputEngine.h"
#	include "SceneManager.h"
#	include "Scene.h"

#	include "ScriptEngine.h"
#	include "ScheduleManager.h"

#	include "Player.h"

#	include "Sprite.h"
#	include "Animation.h"
#	include "HotSpot.h"
#	include "Arrow.h"
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

		static void setCamera3DPosition( float x, float y, float z )
		{
			Holder<Player>::hostage()
				->setCamera3DPosition( mt::vec3f(x, y, z) );
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

		pybind::interface_<Node>("Node", false)
			.def( "activate", &Node::activate )
			.def( "deactivate", &Node::deactivate )
			.def( "setName", &Node::setName )
			.def( "getName", &Node::getName )
			.def( "addChildren", &Node::addChildren )
			.def( "getChildren", &Node::getChildren )
			;

		pybind::class_<Allocator2D>("Allocator2D", false)
			.def( "getLocalPosition", &Allocator2D::getLocalPosition )
			.def( "getLocalDirection", &Allocator2D::getLocalDirection )

			.def( "getWorldPosition", &Allocator2D::getWorldPosition )
			.def( "getWorldDirection", &Allocator2D::getWorldDirection )

			.def( "setLocalPosition", &Allocator2D::setLocalPosition )
			.def( "setLocalDirection", &Allocator2D::setLocalDirection )
			;
		
		pybind::class_<Renderable2D>("Renderable2D", false)
			.def( "hide", &Renderable2D::hide )
			;

		pybind::class_<SceneNode2D, pybind::bases<Node, Allocator2D, Renderable2D>>("SceneNode2D", false)
			;

		{
			pybind::proxy_<Arrow, pybind::bases<SceneNode2D>>("Arrow", false)
				.def( "setOffsetClick", &Arrow::setOffsetClick )
				.def( "getOffsetClick", &Arrow::getOffsetClick )
				;

			pybind::proxy_<Scene, pybind::no_bases >("Scene", false)
				.def( "layerAppend", &Scene::layerAppend )
				.def( "getNode", &Scene::getNode )
				;

			pybind::class_<HotSpot, pybind::bases<SceneNode2D>>("HotSpot", false)
				.def( "setHotspotListener", &HotSpot::setHotspotListener )
				.def( "addPoint", &HotSpot::addPoint )
				.def( "clearPoints", &HotSpot::clearPoints )
				;

			pybind::class_<Sprite, pybind::bases<SceneNode2D>>("Sprite", false)
				.def( "setImageIndex", &Sprite::setImageIndex )
				.def( "getImageIndex", &Sprite::getImageIndex )
				.def( "setImageResource", &Sprite::setImageResource )
				.def( "getImageResource", &Sprite::getImageResource )
				;

			{
				pybind::class_<Animation, pybind::bases<Sprite>>("Animation", false)
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

		pybind::class_<Player, pybind::no_bases>("Player", false)
			;

		pybind::def( "setCurrentScene", &ScriptMethod::setCurrentScene );
		pybind::def( "getCurrentScene", &ScriptMethod::getCurrentScene );
		pybind::def( "setCamera2DPosition", &ScriptMethod::setCamera2DPosition );
		pybind::def( "setCamera3DPosition", &ScriptMethod::setCamera3DPosition );
		
		pybind::def( "createNodeFromXml", &ScriptMethod::createNodeFromXml );

		pybind::def( "schedule", &ScriptMethod::schedule );
		pybind::def( "scheduleRemove", &ScriptMethod::scheduleRemove );

		pybind::def( "getMouseX", &ScriptMethod::getMouseX );
		pybind::def( "getMouseY", &ScriptMethod::getMouseY );

		pybind::def( "getArrow", &ScriptMethod::getArrow );
	}
}