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

		void addImagePath( const ConstString& _imagePath, const mt::vec2f & _size );

		const ConstString & getFileName() const override;
		const ConstString & getCodecType() const override;

	public:
		inline const ResourceImage::ImageFrame & getImageFrame() const;

	protected:
		bool isValid() const override;

	protected:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;
        		
	protected:
		ResourceImage::ImageFrame m_imageFrame;
	
		struct ImageDesc
		{
			ConstString fileName;
			ConstString fileNameAlpha;
			ConstString fileNameRGB;
			ConstString codecType;
			ConstString codecTypeAlpha;
			ConstString codecTypeRGB;
			mt::vec4f uv;
			mt::vec2f offset;
			mt::vec2f maxSize;
			mt::vec2f size;
			bool isAlpha;
			bool isCombined;
			bool wrapX;
			bool wrapY;
		};
		ImageDesc m_imageDesc;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const ResourceImage::ImageFrame & ResourceImageDefault::getImageFrame( ) const
	{
		return m_imageFrame;
	}
}