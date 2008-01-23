#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

#	include <vector>

class	GeometryInterface;

namespace Menge
{

	class ResourcePhysicGeometry
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourcePhysicGeometry )

	public:
		ResourcePhysicGeometry( const std::string & _name );

		virtual const GeometryInterface * getGeometry( size_t _index ) const = 0;
	};
}