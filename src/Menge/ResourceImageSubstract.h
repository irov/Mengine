#	pragma once

#	include "ResourceImageDefault.h"

#	include "Config/Typedef.h"

namespace Menge
{	
	class ResourceImageSubstract
		: public ResourceImage
	{
		DECLARE_VISITABLE( ResourceImage );

	public:
		ResourceImageSubstract();
        ~ResourceImageSubstract();

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

    protected:
		ConstString m_resourceImageName;
		ResourceImagePtr m_resourceImage;		
	};
}
