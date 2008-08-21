#	pragma once

#	include "Factory.h"

#	include "ObjectDeclare.h"
#	include "ResourceDeclare.h"

#	include "Node.h"
#	include "ResourceReference.h"

#	include <string>

#	define OBJECT_FACTORY( Type )\
	TFactoryNode::registration( MENGE_TEXT(#Type) , &Type::genObject )

#	define RESOURCE_FACTORY( Type )\
	TFactoryResource::registration( MENGE_TEXT(#Type) , &Type::genObject )


namespace Menge
{
	typedef Factory<String, Node *, FactoryGenStruct> TFactoryNode;
	typedef Factory<String, ResourceReference *, ResourceFactoryParam> TFactoryResource;
}