#	pragma once

#	include "ResourcePhysicGeometry.h"

namespace Menge
{
	class ResourcePhysicConvexGeometry
		: public ResourcePhysicGeometry
	{
		RESOURCE_DECLARE( ResourcePhysicConvexGeometry )

	public:
		ResourcePhysicConvexGeometry( const ResourceFactoryParam & _params );

	public:
		const GeometryInterface * getGeometry( unsigned int _index ) const;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		String m_filename;
		GeometryInterface * m_interface;
	};
}