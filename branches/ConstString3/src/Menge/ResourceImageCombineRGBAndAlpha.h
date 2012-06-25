#	pragma once

#	include "ResourceImage.h"

#	include "Config/Typedef.h"

#	include <vector>

namespace Menge
{
	class ResourceImageCombineRGBAndAlpha
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageCombineRGBAndAlpha )

	public:
		ResourceImageCombineRGBAndAlpha();

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

	protected:
		bool isValid() const override;
		bool loadImageFrameCombineRGBAndAlpha_( ImageFrame& _frame, const ConstString& _pakName, const WString& _fileNameRGB, const WString& _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha ) const;
		
		RenderTextureInterface * createTextureRGBAndAlpha_( const ConstString& _pakName, const WString& _fileNameRGB, const WString& _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha ) const;
		void loadRGBAndAlphaData_( unsigned char * _buffer, int _pitch, ImageDecoderInterface * _imageDecoderRGB,  ImageDecoderInterface * _imageDecoderAlpha ) const;
	protected:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		ResourceImage::ImageFrame m_imageFrame;

		struct ImageDesc
		{
			ConstString codecType;

			WString fileNameAlpha;
			ConstString codecTypeAlpha;

			WString fileNameRGB;			
			ConstString codecTypeRGB;

			mt::vec4f uv;
			mt::vec2f offset;
			mt::vec2f maxSize;
			mt::vec2f size;

			bool isAlpha;
			bool wrapX;
			bool wrapY;
		};

		ImageDesc m_imageDesc;
	};
}