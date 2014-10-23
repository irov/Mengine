#	pragma once

#	include "Kernel/ResourceImage.h"

#	include "Config/Typedef.h"
#	include "Core/ColourValue.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	class ResourceImageSolid
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageSolid )

	public:
		ResourceImageSolid();

	public:
		const FilePath & getFilePath() const;
		const ConstString & getCodecType() const;

	public:
		const ColourValue & getColor() const;

	protected:
		bool _isValid() const override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;

	protected:
		ColourValue m_color;
		ConstString m_codecType; //empty
		FilePath m_filePath; //empty
	};
	//////////////////////////////////////////////////////////////////////////
}
