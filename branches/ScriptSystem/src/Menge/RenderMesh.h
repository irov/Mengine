#	pragma once
#	include "SceneNode3D.h"

#	include "RenderEngine.h"

namespace Menge
{

	class ResourceMesh;
	class ResourceMaterial;
	class ResourceImage;

	class RenderMesh 
		: public SceneNode3D
	{
		FACTORABLE_DECLARE( RenderMesh );
	public:
		RenderMesh();
		~RenderMesh();

	public:
		void loader( XmlElement * _xml ) override;

		//void createRenderTarget( const String& _name, const mt::vec2f & _resolution );
		//void setMaterial( const String& _materialName );

	protected:
		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
		void _render( Camera2D * _camera ) override;

		//void compileMaterial_();
		//void releaseMaterial_();

	protected:
		String m_resourceName;
		String m_imageName;
		//String m_materialName;
		ResourceMesh* m_resourceMesh;
		//ResourceMaterial* m_resourceMaterial;
		//TMaterial m_material;
		//RenderImageInterface* m_renderTarget;
		RenderObject* m_renderObjectMesh;
		ResourceImage* m_resourceImage;
	};

}	// namespace Menge
