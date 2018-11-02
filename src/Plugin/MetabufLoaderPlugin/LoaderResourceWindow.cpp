#include "LoaderResourceWindow.h"

#include "Engine/ResourceWindow.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceWindow::LoaderResourceWindow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceWindow::~LoaderResourceWindow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceWindow::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceWindowPtr resource = stdex::intrusive_static_cast<ResourceWindowPtr>(_loadable);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceWindow * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceWindow *>(_meta);

        ConstString resources[ResourceWindow_Count];

        metadata->get_WindowBackground_ResourceImageName( &resources[ResourceWindow_Background] );

        resources[ResourceWindow_Bottom] = metadata->get_WindowBottom_ResourceImageName();
        resources[ResourceWindow_Left] = metadata->get_WindowLeft_ResourceImageName();
        resources[ResourceWindow_LeftBottom] = metadata->get_WindowLeftBottom_ResourceImageName();
        resources[ResourceWindow_LeftTop] = metadata->get_WindowLeftTop_ResourceImageName();
        resources[ResourceWindow_Right] = metadata->get_WindowRight_ResourceImageName();
        resources[ResourceWindow_RightBottom] = metadata->get_WindowRightBottom_ResourceImageName();
        resources[ResourceWindow_RightTop] = metadata->get_WindowRightTop_ResourceImageName();
        resources[ResourceWindow_Top] = metadata->get_WindowTop_ResourceImageName();

        for( uint32_t index = 0; index != ResourceWindow_Count; ++index )
        {
            const ConstString & resourceImageName = resources[index];

            resource->setElementResourceImageName( index, resourceImageName );
        }

        mt::vec2f offsets[ResourceWindow_Count];

        offsets[ResourceWindow_Background] = mt::vec2f( 0.f, 0.f );

        offsets[ResourceWindow_Bottom] = metadata->get_WindowBottom_Offset();
        offsets[ResourceWindow_Left] = metadata->get_WindowLeft_Offset();
        offsets[ResourceWindow_LeftBottom] = metadata->get_WindowLeftBottom_Offset();
        offsets[ResourceWindow_LeftTop] = metadata->get_WindowLeftTop_Offset();
        offsets[ResourceWindow_Right] = metadata->get_WindowRight_Offset();
        offsets[ResourceWindow_RightBottom] = metadata->get_WindowRightBottom_Offset();
        offsets[ResourceWindow_RightTop] = metadata->get_WindowRightTop_Offset();
        offsets[ResourceWindow_Top] = metadata->get_WindowTop_Offset();

        for( uint32_t index = 0; index != ResourceWindow_Count; ++index )
        {
            const mt::vec2f & offset = offsets[index];

            resource->setElementOffset( index, offset );
        }

        return true;
    }
}
