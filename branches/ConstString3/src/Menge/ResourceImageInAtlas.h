#	pragma once

#	include "ResourceImageCombineRGBAndAlpha.h"
#	include "Config/Typedef.h"

#	include <vector>

namespace Menge
{
	class ResourceImageAtlas;

	class ResourceImageInAtlas
		: public ResourceImageCombineRGBAndAlpha
	{
		RESOURCE_DECLARE( ResourceImageInAtlas )

	public:
		ResourceImageInAtlas();

	public:
		RenderTextureInterface * getTexture() const override;
		bool loadFrameData( unsigned char * _buffer, int _pitch );
		
	protected:
		void loader( BinParser * _parser ) override;
		bool loadFrameDataRGBAlpha_( unsigned char * _buffer, int _pitch );
		bool loadFrameDataRGB_( unsigned char * _buffer, int _pitch );
	
	protected:
		bool _compile() override;
		void _release() override;

		ResourceImageAtlas * m_resourceAtlas;
		ConstString m_resourceAtlasName;
	};
}
