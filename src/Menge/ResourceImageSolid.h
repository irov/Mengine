#	pragma once

#	include "Kernel/ResourceImage.h"

#	include "Config/Typedef.h"
#	include "Core/ColourValue.h"

namespace Menge
{
	class ResourceImageSolid
		: public ResourceImage
	{
		DECLARE_VISITABLE();

	public:
		ResourceImageSolid();

	protected:
		bool _isValid() const override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;				
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceImageSolid> ResourceImageSolidPtr;
}
