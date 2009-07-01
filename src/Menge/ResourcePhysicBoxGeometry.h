#	pragma once

#	include "ResourcePhysicGeometry.h"

namespace Menge
{
	class ResourcePhysicBoxGeometry
		: public ResourcePhysicGeometry
	{
		RESOURCE_DECLARE( ResourcePhysicBoxGeometry )

	public:
		ResourcePhysicBoxGeometry();

	public:
	
		const GeometryInterface * getGeometry( unsigned int _index ) const;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		float m_width;
		float m_height;
		float m_depth;

		GeometryInterface * m_interface;
	};
}
