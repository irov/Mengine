#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "ResourceReference.h"
#	include "Core/ColourValue.h"
#	include "Math/mat4.h"

namespace Menge
{
	class ResourceImage;

	class ResourceMaterial
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceMaterial )

	public:
		ResourceMaterial();

	public:
		virtual void loader( BinParser * _parser ) override;

		//const TMaterial& getMaterial() const;
		//TMaterial getMaterial();

		const String & getFilename() const;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void loaderMaterial_( BinParser * _parser );

	protected:
		String m_filename;
		ConstString m_textureName;
		ResourceImage* m_resourceImage;
		//TMaterial m_material;
		ColourValue m_color;
		mt::mat4f m_textureMatrix;
	};

}	// namespace Menge
