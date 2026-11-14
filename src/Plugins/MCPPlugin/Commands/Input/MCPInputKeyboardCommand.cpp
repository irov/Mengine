#include "MCPInputKeyboardCommand.h"

#include "../../Contexts/MCPInputContext.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/UnicodeHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPInputKeyboardCommand::MCPInputKeyboardCommand( MCPInputContext * _context )
        : m_context( _context )
    {
        ConstString text = STRINGIZE_STRING_LOCAL( "text" );
        ConstString down = STRINGIZE_STRING_LOCAL( "down" );
        ConstString up = STRINGIZE_STRING_LOCAL( "up" );
        ConstString press = STRINGIZE_STRING_LOCAL( "press" );

        m_actions.emplace( text, &MCPInputKeyboardCommand::text_ );
        m_actions.emplace( down, &MCPInputKeyboardCommand::down_ );
        m_actions.emplace( up, &MCPInputKeyboardCommand::up_ );
        m_actions.emplace( press, &MCPInputKeyboardCommand::press_ );
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPInputKeyboardCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        const Char * actionValue = _request.params.get( "action", "" );
        ConstString actionName = Helper::stringizeString( actionValue );
        MapActions::const_iterator it = m_actions.find( actionName );

        if( it == m_actions.end() )
        {
            _response->errorMessage = "unknown keyboard action";

            return EMCPCommandStatus::FAILURE;
        }

        Action action = it->second;
        bool successful = (this->*action)( _request.params, _response );

        if( successful == false )
        {
            return EMCPCommandStatus::FAILURE;
        }

        _response->result.set( "action", actionValue );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPInputKeyboardCommand::text_( const jpp::object & _params, MCPCommandResponse * const _response )
    {
        VectorMCPKeyCodes modifiers;
        String error;
        if( m_context->getModifierKeys( _params, &modifiers, &error ) == false )
        {
            _response->errorMessage = error;

            return false;
        }

        if( modifiers.empty() == false )
        {
            _response->errorMessage = "modifiers are not supported for text input";

            return false;
        }

        const Char * text = _params.get( "text", "" );
        WString unicode;
        bool converted = Helper::utf8ToUnicode( text, &unicode );

        if( converted == false )
        {
            _response->errorMessage = "text is not valid UTF-8";

            return false;
        }

        if( unicode.empty() == true )
        {
            _response->errorMessage = "text input must not be empty";

            return false;
        }

        Timestamp timestamp = Helper::getSystemTimestamp();
        float cursorX = m_context->getCursorX();
        float cursorY = m_context->getCursorY();
        const WChar * unicodeValue = unicode.c_str();
        Helper::pushTextEvent( timestamp, cursorX, cursorY, 0.f, unicodeValue );
        _response->result.set( "text", text );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPInputKeyboardCommand::down_( const jpp::object & _params, MCPCommandResponse * const _response )
    {
        if( this->pushKey_( _params, true, _response ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPInputKeyboardCommand::up_( const jpp::object & _params, MCPCommandResponse * const _response )
    {
        if( this->pushKey_( _params, false, _response ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPInputKeyboardCommand::press_( const jpp::object & _params, MCPCommandResponse * const _response )
    {
        if( this->pushKey_( _params, true, _response ) == false )
        {
            return false;
        }

        if( this->pushKey_( _params, false, _response ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPInputKeyboardCommand::pushKey_( const jpp::object & _params, bool _down, MCPCommandResponse * const _response )
    {
        const Char * key = _params.get( "key", "" );
        EKeyCode code = m_context->getKeyCode( key );

        if( code == KC_UNASSIGNED )
        {
            _response->errorMessage = "unknown keyboard key";

            return false;
        }

        VectorMCPKeyCodes modifiers;
        String error;
        if( m_context->getModifierKeys( _params, &modifiers, &error ) == false )
        {
            _response->errorMessage = error;

            return false;
        }

        if( _down == true )
        {
            m_context->pushModifierKeys( modifiers, true );
        }

        Timestamp timestamp = Helper::getSystemTimestamp();
        float cursorX = m_context->getCursorX();
        float cursorY = m_context->getCursorY();

        InputSpecialData special;
        m_context->getSpecial( modifiers, &special );

        Helper::pushKeyEvent( timestamp, cursorX, cursorY, 0.f, code, _down, false, special );

        if( _down == false )
        {
            m_context->pushModifierKeys( modifiers, false );
        }

        _response->result.set( "key", key );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
