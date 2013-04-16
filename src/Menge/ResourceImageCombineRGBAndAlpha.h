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
        const FilePath & getFileRGBName() const;
        const ConstString & getCodecRGBType() const;

		const FilePath & getFileAlphaName() const;
		const ConstString & getCodecAlphaType() const;

	protected:
		bool _isValid() const override;
		
		bool loadImageFrameCombineRGBAndAlpha_( const ConstString& _pakName, const FilePath& _fileNameRGB, const FilePath& _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha );
		RenderTextureInterface * createTextureRGBAndAlpha_( const ConstString& _pakName, const FilePath& _fileNameRGB, const FilePath& _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha ) const;
        bool loadRGBData_( unsigned char * _buffer, int _pitch, ImageDecoderInterface * _imageDecoderRGB ) const;
		bool loadAlphaData_( unsigned char * _buffer, int _pitch, ImageDecoderInterface * _imageDecoderAlpha ) const;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;

	protected:
        FilePath m_fileNameAlpha;
        ConstString m_codecTypeAlpha;

        FilePath m_fileNameRGB;			
        ConstString m_codecTypeRGB;
	};
}