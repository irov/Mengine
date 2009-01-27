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
	class RenderImageInterface;

	class ResourceWindow
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceWindow )
	public:
		ResourceWindow( const ResourceFactoryParam & _params );
		virtual ~ResourceWindow();
		
		void loader( XmlElement * _xml ) override;

		RenderImageInterface* getImage( int _type );

		const String & getBackgroundImage() const;
		const String & getLeftTopImage() const;
		const String & getTopImage() const;
		const String & getRightTopImage() const;
		const String & getRightImage() const;
		const String & getRightBottomImage() const;
		const String & getBottomImage() const;
		const String & getLeftBottomImage() const;
		const String & getLeftImage() const;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		String m_imagePath[MAX_WINDOW_ELEMENTS];
		RenderImageInterface* m_renderImage[MAX_WINDOW_ELEMENTS];
	};
}	// namespace Menge
