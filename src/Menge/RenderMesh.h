#	pragma once
#	include "SceneNode3D.h"

#	include "RenderEngine.h"

namespace Menge
{

	class ResourceMesh;
	class ResourceMaterial;

	class RenderMesh 
		: public SceneNode3D
	{
		OBJECT_DECLARE( RenderMesh );
	public:
		RenderMesh();
		virtual ~RenderMesh();

		void loader( XmlElement * _xml ) override;
		void _render( unsigned int _debugMask ) override;

		void createRenderTarget( const String& _name, const mt::vec2f & _resolution );
		void setMaterial( const String& _materialName );

	protected:
		bool _compile() override;
		void _release() override;

		void compileMaterial_();
		void releaseMaterial_();

	protected:
		String m_resourceName;
		String m_materialName;
		ResourceMesh* m_resourceMesh;
		ResourceMaterial* m_resourceMaterial;
		TMaterial m_material;
		RenderImageInterface* m_renderTarget;
	};

}	// namespace Menge