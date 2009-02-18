#	include "ResourcePhysicGeometry.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "PhysicEngine.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourcePhysicGeometry );
	//////////////////////////////////////////////////////////////////////////
	ResourcePhysicGeometry::ResourcePhysicGeometry( const ResourceFactoryParam & _params )
		: ResourceReference( _params )
	{
	}
}
