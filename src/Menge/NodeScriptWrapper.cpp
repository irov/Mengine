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

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	namespace ScriptMethod
	{
		void setCurrentScene( const std::string & _name )
		{
			Holder<Player>::hostage()
					->setCurrentScene( _name );
		}

		PyObject * createNode( const std::string & _type, PyObject * _params  )
		{
			Node * node = SceneManager::createNode( _type );

			PyObject * pyNode = 
				Holder<ScriptEngine>::hostage()
				->wrapp( node );

			//PyObject * pyNode = pybind::ptr( node, *type );

			if( pyNode == 0 )
			{
				delete node;
				return pybind::ret_none();
			}

			pybind::incref( pyNode );

			if( pybind::list_check( _params ) )
			{
				TiXmlElement * node_xml = TiXmlElementCreate("Node");	

				for( size_t it = 0, it_end = pybind::list_size( _params ); it != it_end; ++it )
				{
					PyObject * dict = pybind::list_getitem( _params, it );

					if( pybind::dict_check( dict ) == false )
					{
						continue;
					}

					PyObject *py_key, *py_value;
					size_t pos = 0;

					while( pybind::dict_next( dict, &pos, &py_key, &py_value) ) 
					{
						std::string key = pybind::extract<std::string>( py_key );

						TiXmlElement * key_xml = TiXmlElementCreate( key );

						PyObject *py_value_key, *py_value_value;
						size_t value_pos = 0;

						while( pybind::dict_next( py_value, &value_pos, &py_value_key, &py_value_value) )
						{
							std::string value_key = pybind::extract<std::string>( py_value_key );
							std::string value_value = pybind::extract<std::string>( py_value_value );

							TiXmlElementSetAttribute( key_xml, value_key, value_value );
						}

						TiXmlElementInsertEndChild( node_xml, key_xml );
						TiXmlElementRemove( key_xml );
					}
				}

				node->loader( node_xml );

				TiXmlElementRemove( node_xml );
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


		pybind::interface_<Node>("Node")
			;

		pybind::class_<Allocator2D>("Allocator2D")
			;
		
		pybind::class_<Renderable2D>("Renderable2D")
			.def( "hide", &Renderable2D::hide )
			;

		pybind::class_<SceneNode2D, pybind::bases<Node, Allocator2D, Renderable2D>>("SceneNode2D")
			.def( "activate", &SceneNode2D::activate )
			.def( "setName", &SceneNode2D::setName )
			.def( "getName", &SceneNode2D::getName )
			.def( "addChildren", &SceneNode2D::addChildren )
			;

		{
			pybind::proxy_<Arrow, pybind::bases<SceneNode2D>>("Arrow")
				;

			pybind::proxy_<Scene, pybind::no_bases>("Scene")
				.def( "layerAppend", &Scene::layerAppend )
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
		pybind::def( "createNode", &ScriptMethod::createNode );
	}
}