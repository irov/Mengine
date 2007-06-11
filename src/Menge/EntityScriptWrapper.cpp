#	include "ScriptDeclarationDefine.h"

#	include "Entity.h"

namespace Menge
{
	void addEntity( Entity * _en )
	{
		_en->activate();
	}

	REGISTER_SCRIPT_CLASS( Menge, Entity )
	{
		boost::python::class_<Node, boost::noncopyable>("Node", boost::python::no_init );

		boost::python::class_<NodeImpl, boost::python::bases<Node> >("NodeImpl")
			.def( boost::python::init<>("constructor") )
			.def( "addChildren", &NodeImpl::addChildren )
			.def( "getName", &NodeImpl::getName, boost::python::return_value_policy<boost::python::copy_const_reference>()  )
			;
			

		boost::python::class_<Entity, boost::python::bases<NodeImpl> >("Entity")
			.def( boost::python::init<>("constructor") );

		boost::python::def( "addEntity"
			, &addEntity
			, boost::python::arg("entity")
			);
	}
}