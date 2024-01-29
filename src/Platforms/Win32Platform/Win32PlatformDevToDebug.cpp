#include "Win32PlatformDevToDebug.h"

#include "Interface/ApplicationInterface.h"

#include "Plugins/DevToDebugPlugin/DevToDebugInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/PrototypeHelper.h"

#include "Config/StdIO.h"
#include "Config/StdIntTypes.h"

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
        DevToDebugTabInterfacePtr tab = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugTab" ), MENGINE_DOCUMENT_FACTORABLE );

        DevToDebugWidgetInterfacePtr text1 = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetText" ), MENGINE_DOCUMENT_FACTORABLE );

        text1->setId( STRINGIZE_STRING_LOCAL( "text1" ) );

        DevToDebugPropertyInterfacePtr text1_color = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstColor" ), MENGINE_DOCUMENT_FACTORABLE );

        UnknownDevToDebugPropertyConstColorInterfacePtr unknown_text1_color = text1_color->getUnknown();
        unknown_text1_color->setValue( Helper::makeColorF( 1.f, 0.f, 0.f, 1.f ) );

        text1->setDataProperty( STRINGIZE_STRING_LOCAL( "color" ), text1_color );

        DevToDebugPropertyInterfacePtr text1_content = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstString" ), MENGINE_DOCUMENT_FACTORABLE );

        UnknownDevToDebugPropertyConstStringInterfacePtr unknown_text1_content = text1_content->getUnknown();
        unknown_text1_content->setValue( "Hello World!" );

        text1->setDataProperty( STRINGIZE_STRING_LOCAL( "content" ), text1_content );

        tab->addWidget( text1 );

        DevToDebugWidgetInterfacePtr btn_change = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_FACTORABLE );

        btn_change->setId( STRINGIZE_STRING_LOCAL( "btn_change" ) );

        DevToDebugPropertyInterfacePtr btn_change_title = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstString" ), MENGINE_DOCUMENT_FACTORABLE );
         
        UnknownDevToDebugPropertyConstStringInterfacePtr unknown_btn_change_title = btn_change_title->getUnknown();
        unknown_btn_change_title->setValue( "Change" );

        btn_change->setDataProperty( STRINGIZE_STRING_LOCAL( "title" ), btn_change_title );

        UnknownDevToDebugWidgetButtonInterfacePtr unknown_btn_change = btn_change->getDynamicUnknown();

        unknown_btn_change->setClickEvent( [unknown_text1_content]()
        {
            Char buffer[64] = {'\0'};
            MENGINE_SNPRINTF( buffer, 64, "Bye Bye! %" MENGINE_PRIu64 ""
                , Helper::getTimestamp()
            );

            unknown_text1_content->setValue( buffer );
        } );

        tab->addWidget( btn_change );

        DevToDebugWidgetInterfacePtr btn_quit = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_FACTORABLE );

        btn_quit->setId( STRINGIZE_STRING_LOCAL( "btn_quit" ) );

        DevToDebugPropertyInterfacePtr btn_quit_title = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstString" ), MENGINE_DOCUMENT_FACTORABLE );

        UnknownDevToDebugPropertyConstStringInterfacePtr unknown_btn_quit_title = btn_quit_title->getUnknown();
        unknown_btn_quit_title->setValue( "Quit" );

        btn_quit->setDataProperty( STRINGIZE_STRING_LOCAL( "title" ), btn_quit_title );

        UnknownDevToDebugWidgetButtonInterfacePtr unknown_btn_quit = btn_quit->getDynamicUnknown();

        unknown_btn_quit->setClickEvent( []()
        {
            APPLICATION_SERVICE()
                ->quit();
        } );

        tab->addWidget( btn_quit );

        DevToDebugWidgetInterfacePtr checkbox_test = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetCheckbox" ), MENGINE_DOCUMENT_FACTORABLE );

        checkbox_test->setId( STRINGIZE_STRING_LOCAL( "checkbox_test" ) );

        DevToDebugPropertyInterfacePtr checkbox_test_text = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstString" ), MENGINE_DOCUMENT_FACTORABLE );

        UnknownDevToDebugPropertyConstStringInterfacePtr unknown_checkbox_test_text = checkbox_test_text->getUnknown();
        unknown_checkbox_test_text->setValue( "super" );

        checkbox_test->setDataProperty( STRINGIZE_STRING_LOCAL( "text" ), checkbox_test_text );

        DevToDebugPropertyInterfacePtr checkbox_test_value = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugPropertyConstBoolean" ), MENGINE_DOCUMENT_FACTORABLE );

        UnknownDevToDebugPropertyConstBooleanInterfacePtr unknown_checkbox_test_value = checkbox_test_value->getUnknown();
        unknown_checkbox_test_value->setValue( true );

        checkbox_test->setDataProperty( STRINGIZE_STRING_LOCAL( "value" ), checkbox_test_value );

        UnknownDevToDebugWidgetCheckboxInterfacePtr unknown_checkbox_test = checkbox_test->getDynamicUnknown();

        unknown_checkbox_test->setChangeEvent( []( bool _value )
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