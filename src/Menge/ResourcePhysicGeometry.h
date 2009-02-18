#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

#	include <vector>

namespace Menge
{
	class	GeometryInterface;

	class ResourcePhysicGeometry
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourcePhysicGeometry )

	public:
		ResourcePhysicGeometry( const ResourceFactoryParam & _params );

		virtual const GeometryInterface * getGeometry( unsigned int _index ) const{return 0;};
	};
}
