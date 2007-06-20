#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "ScriptObject.h"

#	include "Holder.h"

#	include "Player.h"

#	include "Entity.h"

#	include "Scene.h"

namespace Menge
{
	SCRIPT_CLASS_WRAPPING( Scene );

	namespace PlayerMethod
	{
		void setCurrentScene( const std::string & _name )
		{
			Holder<Player>::hostage()
				->setCurrentScene( _name );
		}
	}

	REGISTER_SCRIPT_CLASS( Menge, Entity )
	{
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

		boost::python::class_<Node, boost::noncopyable>("Node", boost::python::no_init );

		boost::python::class_<NodeImpl, boost::python::bases<Node> >("NodeImpl")
			.def( boost::python::init<>("constructor") )
			.def( "addChildren", &NodeImpl::addChildren )
			.def( "getName", &NodeImpl::getName, boost::python::return_value_policy<boost::python::copy_const_reference>()  )
			;

		boost::python::class_<Allocator2D, boost::python::bases<NodeImpl> >("Allocator2D")
			.def( boost::python::init<>("constructor") )
			;

		boost::python::class_<Entity, boost::python::bases<Allocator2D> >("Entity")
			.def( boost::python::init<>("constructor") )
			.def( "setLayer", &Entity::setLayer )
			;

		boost::python::class_<Scene, boost::python::bases<NodeImpl> >("Scene")
			.def( "createEntity", &Scene::createEntity, boost::python::return_value_policy<boost::python::reference_existing_object>() )
			;

		boost::python::class_<Player>("Player");

		boost::python::def( "setCurrentScene", &PlayerMethod::setCurrentScene );
	}
}