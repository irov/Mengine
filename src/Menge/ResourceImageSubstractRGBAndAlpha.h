#	pragma once

#	include "Kernel/ResourceImage.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class ResourceImageSubstractRGBAndAlpha
		: public ResourceImage
	{
		DECLARE_VISITABLE( ResourceImage );

	public:
		ResourceImageSubstractRGBAndAlpha();
        ~ResourceImageSubstractRGBAndAlpha();

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
		ResourceImagePtr m_resourceImageRGB;

		ConstString m_resourceImageAlphaName;
		ResourceImagePtr m_resourceImageAlpha;
	};
}