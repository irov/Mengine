#	pragma once

#	include "Kernel/ResourceImage.h"

#	include "Config/Typedef.h"
#	include "Core/ColourValue.h"

namespace Menge
{
	class ResourceImageSolid
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageSolid )

	public:
		ResourceImageSolid();

	public:


	protected:
		bool _isValid() const override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;				
	};
	//////////////////////////////////////////////////////////////////////////
}
