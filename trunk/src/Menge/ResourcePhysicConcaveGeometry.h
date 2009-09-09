#	pragma once

#	include "ResourcePhysicGeometry.h"

namespace Menge
{
	class ResourcePhysicConcaveGeometry
		: public ResourcePhysicGeometry
	{
		RESOURCE_DECLARE( ResourcePhysicConcaveGeometry )

	public:
		ResourcePhysicConcaveGeometry();

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
