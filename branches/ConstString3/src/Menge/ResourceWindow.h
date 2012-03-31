#	pragma once

#	include "ResourceReference.h"

#	define MAX_WINDOW_ELEMENTS 9
		// 0 - Background
		// 1 - LeftTop
		// 2 - Top
		// 3 - RightTop
		// 4 - Right
		// 5 - RightBottom
		// 6 - Bottom
		// 7 - LeftBottom
		// 8 - Left

namespace Menge
{
	class RenderTextureInterface;

	struct WindowImageDesc
	{
		ConstString path;
		ConstString codec;
		RenderTextureInterface * image;
	};

	class ResourceWindow
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceWindow )
	public:
		ResourceWindow();
		~ResourceWindow();
		
	public:
		void loader( BinParser * _parser ) override;

		RenderTextureInterface* getImage( int _type );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		WindowImageDesc m_images[MAX_WINDOW_ELEMENTS];
	};
}	// namespace Menge
