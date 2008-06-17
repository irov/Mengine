#	pragma once
#	include "SceneNode3D-649.h"

#	include "RenderEngine.h"

namespace Menge
{

	class ResourceMesh;

	class RenderMesh 
		: public SceneNode3D_
	{
		OBJECT_DECLARE( RenderMesh );
	public:
		RenderMesh();
		virtual ~RenderMesh();

		void loader( XmlElement * _xml ) override;
		void _render() override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		String m_resourceName;
		ResourceMesh* m_resourceMesh;
		//std::vector<TVertex> m_vertexData;
	};

}	// namespace Menge