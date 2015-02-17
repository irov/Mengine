#	pragma once

#	include "Kernel/ResourceImage.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class ResourceImageSubstractRGBAndAlpha
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageSubstractRGBAndAlpha )

	public:
		ResourceImageSubstractRGBAndAlpha();

	public:
        const ConstString & getResourceRGBName() const;
		const ConstString & getResourceAlphaName() const;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		ConstString m_resourceImageRGBName;
		ResourceImage * m_resourceImageRGB;

		ConstString m_resourceImageAlphaName;
		ResourceImage * m_resourceImageAlpha;
	};
}