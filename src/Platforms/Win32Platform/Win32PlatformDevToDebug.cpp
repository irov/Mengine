#include "Win32PlatformDevToDebug.h"

#include "Interface/ApplicationInterface.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Plugins/DevToDebugPlugin/DevToDebugInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/TimeHelper.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/Logger.h"

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

        DevToDebugPropertyInterfacePtr text1_color = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstColor" ), MENGINE_DOCUMENT_FACTORABLE );

        UnknownDevToDebugPropertyConstColorInterfacePtr unknown_text1_color = text1_color->getUnknown();
        unknown_text1_color->setValue( Helper::makeColorF( 1.f, 0.f, 0.f, 1.f ) );

        unknown_text1->setDataProperty( STRINGIZE_STRING_LOCAL( "color" ), text1_color );

        DevToDebugPropertyInterfacePtr text1_content = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstString" ), MENGINE_DOCUMENT_FACTORABLE );

        UnknownDevToDebugPropertyConstStringInterfacePtr unknown_text1_content = text1_content->getUnknown();
        unknown_text1_content->setValue( "Hello World!" );

        unknown_text1->setDataProperty( STRINGIZE_STRING_LOCAL( "content" ), text1_content );

        tab->addWidget( text1 );

        DevToDebugWidgetInterfacePtr btn_change = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_FACTORABLE );

        btn_change->setId( STRINGIZE_STRING_LOCAL( "btn_change" ) );

        UnknownDevToDebugWidgetButtonInterfacePtr unknown_btn_change = btn_change->getUnknown();

        DevToDebugPropertyInterfacePtr btn_change_title = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstString" ), MENGINE_DOCUMENT_FACTORABLE );

        UnknownDevToDebugPropertyConstStringInterfacePtr unknown_btn_change_title = btn_change_title->getUnknown();
        unknown_btn_change_title->setValue( "Change" );

        unknown_btn_change->setDataProperty( STRINGIZE_STRING_LOCAL( "title" ), btn_change_title );

        unknown_btn_change->setClickEvent( [unknown_text1_content]()
        {
            Char buffer[256];
            MENGINE_SPRINTF( buffer, "Bye Bye! %llu"
                , Helper::getTimeMilliseconds()
            );

            unknown_text1_content->setValue( buffer );
        } );

        tab->addWidget( btn_change );

        DevToDebugWidgetInterfacePtr btn_quit = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_FACTORABLE );

        btn_quit->setId( STRINGIZE_STRING_LOCAL( "btn_quit" ) );

        UnknownDevToDebugWidgetButtonInterfacePtr unknown_btn_quit = btn_quit->getUnknown();

        DevToDebugPropertyInterfacePtr btn_quit_title = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstString" ), MENGINE_DOCUMENT_FACTORABLE );

        UnknownDevToDebugPropertyConstStringInterfacePtr unknown_btn_quit_title = btn_quit_title->getUnknown();
        unknown_btn_quit_title->setValue( "Quit" );

        unknown_btn_quit->setDataProperty( STRINGIZE_STRING_LOCAL( "title" ), btn_quit_title );

        unknown_btn_quit->setClickEvent( []()
        {
            APPLICATION_SERVICE()
                ->quit();
        } );

        tab->addWidget( btn_quit );

        DevToDebugWidgetInterfacePtr checkbox_test = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetCheckbox" ), MENGINE_DOCUMENT_FACTORABLE );

        checkbox_test->setId( STRINGIZE_STRING_LOCAL( "checkbox_test" ) );

        UnknownDevToDebugWidgetCheckboxInterfacePtr unknown_checkbox_test = checkbox_test->getUnknown();

        DevToDebugPropertyInterfacePtr checkbox_test_text = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstString" ), MENGINE_DOCUMENT_FACTORABLE );

        UnknownDevToDebugPropertyConstStringInterfacePtr unknown_checkbox_test_text = checkbox_test_text->getUnknown();
        unknown_checkbox_test_text->setValue( "super" );

        unknown_checkbox_test->setDataProperty( STRINGIZE_STRING_LOCAL( "text" ), checkbox_test_text );

        DevToDebugPropertyInterfacePtr checkbox_test_value = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstBoolean" ), MENGINE_DOCUMENT_FACTORABLE );

        UnknownDevToDebugPropertyConstBooleanInterfacePtr unknown_checkbox_test_value = checkbox_test_value->getUnknown();
        unknown_checkbox_test_value->setValue( true );

        unknown_checkbox_test->setDataProperty( STRINGIZE_STRING_LOCAL( "value" ), checkbox_test_value );

        unknown_checkbox_test->setClickEvent( []( bool _value )
        {
            LOGGER_ERROR( "checkbox: %s"
                , _value == true ? "true" : "false"
            );
        } );

        tab->addWidget( checkbox_test );

        DEVTODEBUG_SERVICE()
            ->addTab( STRINGIZE_STRING_LOCAL( "Platform" ), tab );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}