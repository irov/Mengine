#	pragma once

#	include "Kernel/ResourceReference.h"
#	include "ResourceImage.h"

namespace Menge
{
    enum EResourceWindow
    {
        ResourceWindow_Background = 0,
        ResourceWindow_LeftTop = 1,
        ResourceWindow_Top = 2,
        ResourceWindow_RightTop = 3,
        ResourceWindow_Right = 4,
        ResourceWindow_RightBottom = 5,
        ResourceWindow_Bottom = 6,
        ResourceWindow_LeftBottom = 7,
        ResourceWindow_Left = 8,
        ResourceWindow_Count = 9,
        __ResourceWindow__
    };

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
		bool _loader( const Metabuf::Metadata * _parser ) override;

    public:
        ResourceImage * getResource( int _type );
		const mt::vec2f &  getOffset( int _type );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		WindowElement m_images[ResourceWindow_Count];
	};
}	// namespace Menge
