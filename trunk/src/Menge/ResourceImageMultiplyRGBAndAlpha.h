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
        const FilePath & getFileRGBName() const;
        const ConstString & getCodecRGBType() const;

		const FilePath & getFileAlphaName() const;
		const ConstString & getCodecAlphaType() const;

	protected:
		bool _isValid() const override;

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