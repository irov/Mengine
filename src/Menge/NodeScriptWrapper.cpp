#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "SceneManager.h"
#	include "Scene.h"

#	include "ScriptEngine.h"

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
		void setCurrentScene( const std::string & _name )
		{
			printf("set current scene '%s'\n", _name.c_str() );
			Holder<Player>::hostage()
					->setCurrentScene( _name );
		}

		void setCamera2DPosition( float x, float y )
		{
			Holder<Player>::hostage()
				->setCamera2DPosition( mt::vec2f(x, y) );
		}

		void setCamera3DPosition( float x, float y, float z )
		{
			Holder<Player>::hostage()
				->setCamera3DPosition( mt::vec3f(x, y, z) );
		}

		PyObject * createNodeFromXml( PyObject * _params  )
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
	SCRIPT_CLASS_WRAPPING( Player );	

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
			.def( "setName", &Node::setName )
			.def( "getName", &Node::getName )
			.def( "addChildren", &Node::addChildren )
			.def( "getChildren", &Node::getChildren )
			;

		static struct NodePtrExtract
			: public pybind::interface_<Node>::extract_ptr_type
		{
			PyObject * wrapp( Node * _node ) override
			{
				if( _node == 0 )
				{
					return 0;
				}

				Scriptable * scriptable = _node->getScriptable();
				PyObject * pyObj = scriptable->getScript();
				return pyObj;
			}
		} s_node_ptr_extract;

		pybind::class_<Allocator2D>("Allocator2D")
			.def( "getLocalPosition", &Allocator2D::getLocalPosition )
			.def( "getLocalDirection", &Allocator2D::getLocalDirection )

			.def( "getWorldPosition", &Allocator2D::getWorldPosition )
			.def( "getWorldDirection", &Allocator2D::getWorldDirection )

			.def( "setLocalPosition", &Allocator2D::setLocalPosition )
			.def( "setLocalDirection", &Allocator2D::setLocalDirection )
			;
		
		pybind::class_<Renderable2D>("Renderable2D")
			.def( "hide", &Renderable2D::hide )
			;

		pybind::class_<SceneNode2D, pybind::bases<Node, Allocator2D, Renderable2D>>("SceneNode2D")
			;

		{
			pybind::proxy_<Arrow, pybind::bases<SceneNode2D>>("Arrow")
				;

			pybind::proxy_<Scene, pybind::no_bases >("Scene")
				.def( "layerAppend", &Scene::layerAppend )
				.def( "getNode", &Scene::getNode )
				;

			pybind::class_<HotSpot, pybind::bases<SceneNode2D>>("HotSpot")
				.def( "setInputHandler", &HotSpot::setInputHandler )
				;

			pybind::class_<Sprite, pybind::bases<SceneNode2D>>("Sprite")
				.def( "setImageIndex", &Sprite::setImageIndex )
				.def( "getImageIndex", &Sprite::getImageIndex )
				.def( "setImageResource", &Sprite::setImageResource )
				.def( "getImageResource", &Sprite::getImageResource )
				;

			{

				pybind::class_<Animation, pybind::bases<Sprite>>("Animation")
					;

			}

		}		

		pybind::class_<Player, pybind::no_bases>("Player")
			;

		pybind::def( "setCurrentScene", &ScriptMethod::setCurrentScene );
		pybind::def( "setCamera2DPosition", &ScriptMethod::setCamera2DPosition );
		pybind::def( "setCamera3DPosition", &ScriptMethod::setCamera3DPosition );
		
		pybind::def( "createNodeFromXml", &ScriptMethod::createNodeFromXml );
	}
}