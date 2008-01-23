#	pragma once

#	include "ResourcePhysicGeometry.h"

namespace Menge
{
	class ResourcePhysicBoxGeometry
		: public ResourcePhysicGeometry
	{
		RESOURCE_DECLARE( ResourcePhysicBoxGeometry )

	public:
		ResourcePhysicBoxGeometry( const std::string & _name );

	public:
	
		const GeometryInterface * getGeometry( size_t _index ) const;

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