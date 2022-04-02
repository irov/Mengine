#include "TextValidatePlugin.h"

#include "Interface/TextServiceInterface.h"

#include "Kernel/BuildMode.h"
#include "Kernel/Logger.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( TextValidate, Mengine::TextValidatePlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextValidatePlugin::TextValidatePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextValidatePlugin::~TextValidatePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextValidatePlugin::_initializePlugin()
    {
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME, &TextValidatePlugin::notifyBootstrapperInitializeGame_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextValidatePlugin::_finalizePlugin()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextValidatePlugin::notifyBootstrapperInitializeGame_()
    {
        bool OPTION_notextcheck = HAS_OPTION( "notextcheck" );

        if( OPTION_notextcheck == true )
        {
            return;
        }

        bool windowsDebug = Helper::isWindowsDebug();

        if( windowsDebug == false )
        {
            return;
        }

        bool developmentMode = Helper::isDevelopmentMode();

        if( developmentMode == false )
        {
            return;
        }

        LOGGER_MESSAGE_WN( false, true, "Validate Texts... " );

        bool successful = true;

        TEXT_SERVICE()
            ->foreachTextEntry( [&successful]( const TextEntryInterfacePtr & _textEntry )
        {
            const ConstString & textId = _textEntry->getKey();
            const TextFontInterfacePtr & font = _textEntry->getFont();

            if( font == nullptr )
            {
                return;
            }

            size_t text_size;
            const Char * text_value = _textEntry->getValue( &text_size );

            if( font->validateText( textId, text_value, text_size ) == false )
            {
                LOGGER_ERROR( "text '%s' font name '%s' invalid"
                    , textId.c_str()
                    , font->getName().c_str()
                );

                successful = false;
            }
        } );

        if( successful == false )
        {
            LOGGER_MESSAGE( "[FAILURE]" );

            bool OPTION_notextcheckcritical = HAS_OPTION( "notextcheckcritical" );

            if( OPTION_notextcheckcritical == false )
            {
                LOGGER_CRITICAL( "Please fix [text] and restart application!" );

                throw ExceptionNotificationFailed();
            }
        }
        else
        {
            LOGGER_MESSAGE( "[OK]" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
