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

        DevToDebugWidgetInterfacePtr text1 = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetText" ), MENGINE_DOCUMENT_FACTORABLE );

        text1->setId( STRINGIZE_STRING_LOCAL( "text1" ) );

        UnknownDevToDebugWidgetTextInterfacePtr unknown_text1 = text1->getUnknown();
        unknown_text1->setConstText( "Hello World!" );

        tab->addWidget( text1 );

        DevToDebugWidgetInterfacePtr btn_change = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_FACTORABLE );

        btn_change->setId( STRINGIZE_STRING_LOCAL( "btn_change" ) );

        UnknownDevToDebugWidgetButtonInterfacePtr unknown_btn_change = btn_change->getUnknown();
        unknown_btn_change->setTitle( "Change" );

        unknown_btn_change->setClickEvent( [unknown_text1]()
        {
            Char buffer[256];
            MENGINE_SPRINTF( buffer, "Bye Bye! %llu"
                , Helper::getTimeMilliseconds()
            );

            unknown_text1->setConstText( buffer );
        } );

        tab->addWidget( btn_change );

        DevToDebugWidgetInterfacePtr btn_quit = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_FACTORABLE );

        btn_quit->setId( STRINGIZE_STRING_LOCAL( "btn_quit" ) );

        UnknownDevToDebugWidgetButtonInterfacePtr unknown_btn_quit = btn_quit->getUnknown();
        unknown_btn_quit->setTitle( "Quit" );

        unknown_btn_quit->setClickEvent( []()
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