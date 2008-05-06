#	pragma once

#	include "Factory.h"

#	include "ObjectDeclare.h"
#	include "ResourceDeclare.h"

#	include "Node.h"
#	include "ResourceReference.h"

#	include <string>

#	define OBJECT_FACTORY( Type )\
	TFactoryNode::registration( #Type , &Type::genObject )

#	define RESOURCE_FACTORY( Type )\
	TFactoryResource::registration( #Type , &Type::genObject )


namespace Menge
{
	typedef Factory<std::string, Node *, FactoryGenStruct> TFactoryNode;
	typedef Factory<std::string, ResourceReference *, ResourceFactoryParam> TFactoryResource;
}