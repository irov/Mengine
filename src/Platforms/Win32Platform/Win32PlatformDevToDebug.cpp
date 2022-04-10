#include "Win32PlatformDevToDebug.h"

#include "Interface/ApplicationInterface.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Plugins/DevToDebugPlugin/DevToDebugInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/TimeHelper.h"

#include "Config/StdIo.h"

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

        DevToDebugWidgetTextInterfacePtr text1 = PROTOTYPE_SERVICE()
            ->generateVirtualInheritancePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetText" ), MENGINE_DOCUMENT_FACTORABLE );

        text1->setId( STRINGIZE_STRING_LOCAL( "text1" ) );
        text1->setConstText( "Hello World!" );

        tab->addWidget( text1 );

        DevToDebugWidgetButtonInterfacePtr btn_change = PROTOTYPE_SERVICE()
            ->generateVirtualInheritancePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_FACTORABLE );

        btn_change->setId( STRINGIZE_STRING_LOCAL( "btn_change" ) );
        btn_change->setTitle( "Change" );

        btn_change->setClickEvent( [text1]()
        {
            Char buffer[256];
            MENGINE_SPRINTF( buffer, "Bye Bye! %llu"
                , Helper::getTimeMilliseconds()
            );

            text1->setConstText( buffer );
        } );

        tab->addWidget( btn_change );

        DevToDebugWidgetButtonInterfacePtr btn_quit = PROTOTYPE_SERVICE()
            ->generateVirtualInheritancePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_FACTORABLE );

        btn_quit->setId( STRINGIZE_STRING_LOCAL( "btn_quit" ) );
        btn_quit->setTitle( "Quit" );

        btn_quit->setClickEvent( []()
        {
            APPLICATION_SERVICE()
                ->quit();
        } );

        tab->addWidget( btn_quit );

        DEVTODEBUG_SERVICE()
            ->addTab( STRINGIZE_STRING_LOCAL( "Platform" ), tab );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}