#	pragma once

#	include "Kernel/ResourceImage.h"

#	include "Config/Typedef.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	class ResourceImageMultiplyRGBAndAlpha
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageMultiplyRGBAndAlpha )

	public:
		ResourceImageMultiplyRGBAndAlpha();

	public:
        const FilePath & getFilePathRGB() const;
        const ConstString & getCodecTypeRGB() const;

		const FilePath & getFilePathAlpha() const;
		const ConstString & getCodecTypeAlpha() const;

	protected:
		bool _isValid() const override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;

	protected:
        FilePath m_filePathAlpha;
        ConstString m_codecTypeAlpha;

        FilePath m_filePathRGB;			
        ConstString m_codecTypeRGB;
	};
}