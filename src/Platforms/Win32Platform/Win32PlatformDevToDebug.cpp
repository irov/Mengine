#include "Win32PlatformDevToDebug.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Plugins/DevToDebugPlugin/DevToDebugInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32PlatformDevToDebug::Win32PlatformDevToDebug()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32PlatformDevToDebug::~Win32PlatformDevToDebug()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PlatformDevToDebug::execute()
    {
        DevToDebugTabInterfacePtr tab = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugTab" ), MENGINE_DOCUMENT_FACTORABLE );

        DevToDebugWidgetTextInterfacePtr widgetText = PROTOTYPE_SERVICE()
            ->generateVirtualInheritancePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetText" ), MENGINE_DOCUMENT_FACTORABLE );

        widgetText->setConstText( "Hello World!" );

        tab->addWidget( widgetText );

        DevToDebugWidgetButtonInterfacePtr widgetButton = PROTOTYPE_SERVICE()
            ->generateVirtualInheritancePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_FACTORABLE );

        widgetButton->setTitle( "Quit" );

        tab->addWidget( widgetText );

        DEVTODEBUG_SERVICE()
            ->addTab( STRINGIZE_STRING_LOCAL( "Platform" ), tab );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}