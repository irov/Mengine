#	pragma once

#	include "ResourceReference.h"
#	include "Interface/RenderSystemInterface.h"
#	include <vector>

class MeshInterface;

namespace Menge
{
	class ResourceMesh
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceMesh )

	public:
		ResourceMesh( const ResourceFactoryParam & _params );

	public:
		virtual void loader( XmlElement * _xml ) override;

	public:

		const String& getMeshName() const;
		//const std::string & getSkeletonName() const;
		//const std::string & getMaterialName() const;
		virtual const std::vector<TVertex>& getVertexData();
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