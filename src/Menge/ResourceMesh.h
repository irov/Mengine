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

		MeshInterface * getMesh() const;
	
	public:
		void loader( XmlElement * _xml ) override;


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