#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "ScriptObject.h"

#	include "SceneManager.h"
#	include "Scene.h"

#	include "ScriptEngine.h"

#	include "Player.h"

#	include "Sprite.h"
#	include "Animation.h"
#	include "HotSpot.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	SCRIPT_CLASS_WRAPPING( Scene );
	SCRIPT_CLASS_WRAPPING( Sprite );
	SCRIPT_CLASS_WRAPPING( Animation );
	SCRIPT_CLASS_WRAPPING( HotSpot );

	namespace ScriptMethod
	{
		void setCurrentScene( const std::string & _name )
		{
			Holder<Player>::hostage()
				->setCurrentScene( _name );
		}

		PyObject * createNode( const std::string & _type, PyObject * _params  )
		{
		//	Node * node = SceneManager::createNode( _type );

		//	if( node->isScriptable() == false )
		//	{
		//		node->destroy();
		//		Py_RETURN_NONE;
		//	}

		//	if( PyList_Check( _params ) )
		//	{
		//		TiXmlElement node_xml("Node");	

		//		for( Py_ssize_t it = 0, it_end = PyList_Size( _params ); it != it_end; ++it )
		//		{
		//			PyObject * dict = PyList_GetItem( _params, it );

		//			if( PyDict_Check( dict ) == 0 )
		//			{
		//				continue;
		//			}

		//			PyObject *py_key, *py_value;
		//			Py_ssize_t pos = 0;

		//			Py_ssize_t dict_size = PyDict_Size( dict );

		//			while( PyDict_Next( dict, &pos, &py_key, &py_value) ) 
		//			{
		//				std::string key = PyString_AsString( py_key );

		//				TiXmlElement key_xml( key );

		//				PyObject *py_value_key, *py_value_value;
		//				Py_ssize_t value_pos = 0;

		//				while( PyDict_Next( py_value, &value_pos, &py_value_key, &py_value_value) )
		//				{
		//					std::string value_key = PyString_AsString( py_value_key );
		//					std::string value_value = PyString_AsString( py_value_value );

		//					key_xml.SetAttribute( value_key, value_value );
		//				}

		//				node_xml.InsertEndChild( key_xml );
		//			}
		//		}

		//		node->loader( &node_xml );
		//	}

		//	PyObject * pyNode = node->getScriptable();

		//	return pyNode;
			return 0;
		}
	}

	REGISTER_SCRIPT_CLASS( Menge, Node, Base )
	{
		boost::python::return_value_policy<boost::python::reference_existing_object> retValuePolice;

		boost::python::class_<mt::vec2f>("vec2f")
			.def( boost::python::init<float,float>() )
			.def( boost::python::self + boost::python::self )	// __add__
			.def( boost::python::self - boost::python::self )          // __radd__
			.def( boost::python::self * float() )           // __sub__
			.def( boost::python::self / float() )          // __sub__
			.def( float() * boost::python::self )         // __iadd__
			.def( boost::python::self += boost::python::self )
			.def( boost::python::self -= boost::python::self )
			.def( boost::python::self *= float() )
			.def( boost::python::self /= float() )
			;


		//boost::python::to_python_converter<
		//	ScriptObject *,
		//	ScriptObjectConverter>();

		boost::python::class_<ScriptObject>("ScriptObject" )
			.def( boost::python::init<boost::python::object>() )
			;

		boost::python::class_<Node, boost::noncopyable>("Node", boost::python::no_init )
			;		

		boost::python::class_<Allocator2D>("Allocator2D")
			;

		boost::python::class_<Renderable2D>("Renderable2D")
			.def( "hide", &Renderable2D::hide )
			;

		boost::python::class_<SceneNode2D, boost::python::bases<Node, Allocator2D, Renderable2D>  >("SceneNode2D")
			.def( "activate", &SceneNode2D::activate )
			.def( "setName", &SceneNode2D::setName )
			.def( "getName", &SceneNode2D::getName, retValuePolice  )
			.def( "addChildren", &SceneNode2D::addChildren )
			;		

		{
			boost::python::class_<Scene, boost::python::bases<SceneNode2D> >("Scene", boost::python::no_init )
				.def( "layerAppend", &Scene::layerAppend )
				;

			boost::python::class_<HotSpot, boost::python::bases<SceneNode2D> >("HotSpot")
				.def( boost::python::init<>("constructor") )
				.def( "setMouseLeftClickEvent", &HotSpot::setMouseLeftClickEvent )
				;

			boost::python::class_<Sprite, boost::python::bases<SceneNode2D> >("Sprite")
				.def( boost::python::init<>("constructor") )
				.def( "setImageIndex", &Sprite::setImageIndex )
				.def( "getImageIndex", &Sprite::getImageIndex )
				;
			{
				boost::python::class_<Animation, boost::python::bases<Sprite> >("Animation")
					.def( boost::python::init<>("constructor") )
					;
			}

		}		

		boost::python::class_<Player>("Player");

		boost::python::def( "setCurrentScene", &ScriptMethod::setCurrentScene );
		boost::python::def( "createNode", &ScriptMethod::createNode );
	}
}