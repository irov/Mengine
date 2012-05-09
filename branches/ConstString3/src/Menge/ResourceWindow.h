#	pragma once

#	include "ResourceReference.h"
#	include "ResourceImage.h"
#	define MAX_WINDOW_ELEMENTS 9

#	define ResourceWindow_Background 0
#	define ResourceWindow_LeftTop 1
#	define ResourceWindow_Top 2
#	define ResourceWindow_RightTop 3
#	define ResourceWindow_Right 4
#	define ResourceWindow_RightBottom 5
#	define ResourceWindow_Bottom 6
#	define ResourceWindow_LeftBottom 7
#	define ResourceWindow_Left 8

namespace Menge
{
	class RenderTextureInterface;

	struct WindowElement
	{
		ConstString resourceName;
		ResourceImage * resource;
		mt::vec2f offset;
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
		const mt::vec2f &  getOffset( int _type );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		WindowElement m_images[MAX_WINDOW_ELEMENTS];
	};
}	// namespace Menge
