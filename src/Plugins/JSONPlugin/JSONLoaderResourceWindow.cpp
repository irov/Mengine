#include "JSONLoaderResourceWindow.h"

#include "Interface/ResourceBankInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceWindow::JSONLoaderResourceWindow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceWindow::~JSONLoaderResourceWindow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceWindow::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceWindow * resource = _factorable.getT<ResourceWindow *>();

        const ConstString & groupName = resource->getGroupName();

        ConstString resources[ResourceWindow_Count];

        Helper::getJSONMemberConstString( _json, "WindowBackground", "ResourceImageName", &resources[ResourceWindow_Background] );
        Helper::getJSONMemberConstString( _json, "WindowBottom", "ResourceImageName", &resources[ResourceWindow_Bottom] );
        Helper::getJSONMemberConstString( _json, "WindowLeft", "ResourceImageName", &resources[ResourceWindow_Left] );
        Helper::getJSONMemberConstString( _json, "WindowLeftBottom", "ResourceImageName", &resources[ResourceWindow_LeftBottom] );
        Helper::getJSONMemberConstString( _json, "WindowLeftTop", "ResourceImageName", &resources[ResourceWindow_LeftTop] );
        Helper::getJSONMemberConstString( _json, "WindowRight", "ResourceImageName", &resources[ResourceWindow_Right] );
        Helper::getJSONMemberConstString( _json, "WindowRightBottom", "ResourceImageName", &resources[ResourceWindow_RightBottom] );
        Helper::getJSONMemberConstString( _json, "WindowRightTop", "ResourceImageName", &resources[ResourceWindow_RightTop] );
        Helper::getJSONMemberConstString( _json, "WindowTop", "ResourceImageName", &resources[ResourceWindow_Top] );

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

        Helper::getJSONMemberVec2f( _json, "WindowBottom", "Offset", &offsets[ResourceWindow_Bottom] );
        Helper::getJSONMemberVec2f( _json, "WindowLeft", "Offset", &offsets[ResourceWindow_Left] );
        Helper::getJSONMemberVec2f( _json, "WindowLeftBottom", "Offset", &offsets[ResourceWindow_LeftBottom] );
        Helper::getJSONMemberVec2f( _json, "WindowLeftTop", "Offset", &offsets[ResourceWindow_LeftTop] );
        Helper::getJSONMemberVec2f( _json, "WindowRight", "Offset", &offsets[ResourceWindow_Right] );
        Helper::getJSONMemberVec2f( _json, "WindowRightBottom", "Offset", &offsets[ResourceWindow_RightBottom] );
        Helper::getJSONMemberVec2f( _json, "WindowRightTop", "Offset", &offsets[ResourceWindow_RightTop] );
        Helper::getJSONMemberVec2f( _json, "WindowTop", "Offset", &offsets[ResourceWindow_Top] );

        for( uint32_t index = 0; index != ResourceWindow_Count; ++index )
        {
            const mt::vec2f & offset = offsets[index];

            resource->setElementOffset( index, offset );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
