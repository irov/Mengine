#	pragma once

#	include "ResourceImage.h"

#	include "Config/Typedef.h"

#	include <vector>

namespace Menge
{
	class ResourceImageMultiplyRGBAndAlpha
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageMultiplyRGBAndAlpha )

	public:
		ResourceImageMultiplyRGBAndAlpha();

	public:
        const WString & getFileRGBName() const;
        const ConstString & getCodecRGBType() const;

		const WString & getFileAlphaName() const;
		const ConstString & getCodecAlphaType() const;

    protected:
		bool loadBufferAlpha( unsigned char * _buffer, int _pitch ) override;

	protected:
		bool isValid() const override;

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