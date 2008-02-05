#	pragma once

#	include "ResourceReference.h"

class MeshInterface;

namespace Menge
{
	class ResourceMesh
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceMesh )

	public:
		ResourceMesh( const std::string & _name );

	public:
		void loader( XmlElement * _xml ) override;

	public:

		const std::string & getMeshName() const;
		const std::string & getSkeletonName() const;
		const std::string & getMaterialName() const;

	protected:
		bool _compile() override;
		void _release() override;

	private:

		std::string m_meshName;
		std::string m_skeletonName;
		std::string m_materialName;

		MeshInterface * m_interface;
	};
}