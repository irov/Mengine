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
		bool loadBuffer( unsigned char * _buffer, int _pitch ) override;

	protected:
		bool isValid() const override;
		
		bool loadImageFrameCombineRGBAndAlpha_( ImageFrame& _frame, const ConstString& _pakName, const WString& _fileNameRGB, const WString& _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha ) const;
		RenderTextureInterface * createTextureRGBAndAlpha_( const ConstString& _pakName, const WString& _fileNameRGB, const WString& _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha ) const;
		void loadRGBAndAlphaData_( unsigned char * _buffer, int _pitch, ImageDecoderInterface * _imageDecoderRGB,  ImageDecoderInterface * _imageDecoderAlpha ) const;
	protected:
		void loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
        WString m_fileNameAlpha;
        ConstString m_codecTypeAlpha;

        WString m_fileNameRGB;			
        ConstString m_codecTypeRGB;
	};
}