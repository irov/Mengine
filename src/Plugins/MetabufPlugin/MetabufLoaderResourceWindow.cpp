#include "MetabufLoaderResourceWindow.h"

#include "Interface/ResourceBankInterface.h"

#include "Kernel/AssertionMemoryPanic.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceWindow::MetabufLoaderResourceWindow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceWindow::~MetabufLoaderResourceWindow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceWindow::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceWindow * resource = _factorable.getT<ResourceWindow *>();

        const ConstString & groupName = resource->getGroupName();

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

        ResourceBankInterface * resourceBank = resource->getResourceBank();

        for( uint32_t index = 0; index != ResourceWindow_Count; ++index )
        {
            const ConstString & resourceImageName = resources[index];

            const ResourceImagePtr & resourceImage = resourceBank->getResource( groupName, resourceImageName );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, "'%s' group '%s' invalid get image resource"
                , resource->getName().c_str()
                , resource->getGroupName().c_str()
            );

            resource->setElementResourceImage( index, resourceImage );
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
    //////////////////////////////////////////////////////////////////////////
}
