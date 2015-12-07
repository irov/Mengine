#	pragma once

#	include "Kernel/ResourceReference.h"

#	include "Kernel/ResourceImage.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////////////////
	struct WindowElement
	{
		ConstString resourceName;
		ResourceImagePtr resource;
		mt::vec2f offset;
	};
	//////////////////////////////////////////////////////////////////////////
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
		const ResourceImagePtr & getResource( int _type ) const;
		const mt::vec2f &  getOffset( int _type ) const;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		WindowElement m_images[ResourceWindow_Count];
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceWindow> ResourceWindowPtr;
}	// namespace Menge
