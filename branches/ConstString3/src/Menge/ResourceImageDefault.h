#	pragma once

#	include "ResourceImage.h"

#	include "Config/Typedef.h"

#	include <vector>

namespace Menge
{
	class ResourceImageDefault
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageDefault )

	public:
		ResourceImageDefault();

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

		void addImagePath( const WString& _imagePath, const mt::vec2f & _size );

		const WString & getFileName() const override;
		const ConstString & getCodecType() const override;
		bool loadBuffer( unsigned char * _buffer, int _pitch ) override;

	protected:
		bool isValid() const override;

	protected:
		void loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;
        		
	protected:
        WString m_fileName;
        ConstString m_codecType;                
	};
}