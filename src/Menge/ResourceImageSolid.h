#	pragma once

#	include "ResourceImage.h"
#	include "Config/Typedef.h"
#	include "Core/ColourValue.h"
#	include <vector>

namespace Menge
{
	class ResourceImageSolid
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageSolid )

	public:
		ResourceImageSolid();

	public:
		const FilePath & getFileName() const;
		const ConstString & getCodecType() const;
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
		FilePath m_fileName; //empty
	};
	//////////////////////////////////////////////////////////////////////////
}
