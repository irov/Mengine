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
		const WString & getFileName() const;
		const ConstString & getCodecType() const;
		const ColourValue & getColor() const;

    protected:
		bool loadBufferAlpha( unsigned char * _buffer, int _pitch ) override;

	protected:
		bool isValid() const override;

	protected:
		void loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		ColourValue m_color;
		ConstString m_codecType; //empty
		WString m_fileName; //empty
	};
	//////////////////////////////////////////////////////////////////////////
}
