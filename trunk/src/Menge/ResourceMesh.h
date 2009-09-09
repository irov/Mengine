#	pragma once

#	include "ResourceReference.h"
#	include "Vertex.h"
#	include <vector>

class MeshInterface;

namespace Menge
{
	class ResourceMesh
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceMesh )

	public:
		ResourceMesh();

	public:
		virtual void loader( XmlElement * _xml ) override;

	public:

		const String& getMeshName() const;
		const String& getFileName() const;
		//const std::string & getSkeletonName() const;
		//const std::string & getMaterialName() const;
		virtual const std::vector<Vertex3D>& getVertexData();
		virtual const std::vector<uint16>& getIndexData();
		virtual void addTiming( float _timing ) {};

	protected:
		bool _compile() override;
		void _release() override;

	protected:

		String m_filename;
		String m_meshName;
		//std::string m_skeletonName;
		//std::string m_materialName;
	};
}
