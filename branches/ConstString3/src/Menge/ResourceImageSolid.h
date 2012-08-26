#	pragma once

#	include "ResourceImage.h"
#	include "Config/Typedef.h"
#	include "Core/ColourValue.h"
#	include <vector>

namespace Menge
{
	class ResourceImageSolid
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageSolid )

	public:
		ResourceImageSolid();

	public:
		const mt::vec2f & getMaxSize() const override;
		const mt::vec2f & getSize() const override;
		const mt::vec2f & getOffset() const override;
		const mt::vec4f & getUV() const override;
		const mt::vec4f & getUVImage() const override;

		bool isAlpha() const override;
		bool getWrapX() const override;
		bool getWrapY() const override;

		RenderTextureInterface * getTexture() const override;
		
		const WString & getFileName() const override;
		const ConstString & getCodecType() const override;
		const ColourValue & getColor() const;
		bool loadBuffer( unsigned char * _buffer, int _pitch ) override;

	protected:
		bool isValid() const override;

	protected:
		void loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		ColourValue m_color;
		ConstString m_codecType; //empty
		WString m_fileName; //empty
	};
	//////////////////////////////////////////////////////////////////////////
}
