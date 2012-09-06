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
        const WString & getFileRGBName() const;
        const ConstString & getCodecRGBType() const;

		const WString & getFileAlphaName() const;
		const ConstString & getCodecAlphaType() const;

    protected:
		bool loadBufferAlpha( unsigned char * _buffer, int _pitch ) override;

	protected:
		bool isValid() const override;
		
		bool loadImageFrameCombineRGBAndAlpha_( const ConstString& _pakName, const WString& _fileNameRGB, const WString& _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha );
		RenderTextureInterface * createTextureRGBAndAlpha_( const ConstString& _pakName, const WString& _fileNameRGB, const WString& _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha ) const;
        void loadRGBData_( unsigned char * _buffer, int _pitch, ImageDecoderInterface * _imageDecoderRGB ) const;
		void loadAlphaData_( unsigned char * _buffer, int _pitch, ImageDecoderInterface * _imageDecoderAlpha ) const;

	protected:
		void loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;

	protected:
        WString m_fileNameAlpha;
        ConstString m_codecTypeAlpha;

        WString m_fileNameRGB;			
        ConstString m_codecTypeRGB;
	};
}