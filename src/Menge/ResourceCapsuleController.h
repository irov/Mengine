#	pragma once

#	include "ResourceReference.h"

namespace Menge
{
	class ResourceCapsuleController
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceCapsuleController )

	public:
		ResourceCapsuleController( const ResourceFactoryParam & _params );

	public:
		float getRadius();
		float getHeight();
	
	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		float m_radius;
		float m_height;
	};
}