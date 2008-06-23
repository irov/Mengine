#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "ResourceReference.h"

namespace Menge
{
	class ResourceImage;

	class ResourceMaterial
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceMaterial )

	public:
		ResourceMaterial( const ResourceFactoryParam & _params );

		virtual void loader( XmlElement * _xml ) override;

		const TMaterial& getMaterial() const;
		TMaterial getMaterial();

	protected:
		bool _compile() override;
		void _release() override;

		void loaderMaterial_( XmlElement* _xml );
	protected:
		String m_filename;
		String m_textureName;
		ResourceImage* m_resourceImage;
		TMaterial m_material;
	};

}	// namespace Menge