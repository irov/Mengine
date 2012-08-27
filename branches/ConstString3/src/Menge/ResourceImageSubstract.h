#	pragma once

#	include "ResourceImageDefault.h"
#	include "Config/Typedef.h"

#	include <vector>

namespace Menge
{	
	class ResourceImageSubstract
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageSubstract )

	public:
		ResourceImageSubstract();

	protected:
		void loader( const Metabuf::Metadata * _parser ) override;

    protected:
        bool loadBufferAlpha( unsigned char * _buffer, int _pitch ) override;

	protected:
		bool _compile() override;
		void _release() override;

    protected:
		ResourceImage * m_resourceImage;
		ConstString m_resourceImageName;

        mt::vec4f m_uv;
	};
}
