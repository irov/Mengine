#include "MCPInputContext.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/InputServiceInterface.h"

#include "MCPSceneContext.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/TimestampHelper.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void setModifierKey( InputSpecialData * const _special, EKeyCode _code, bool _down )
        {
            switch( _code )
            {
            case KC_SHIFT:
                _special->isShift = _down;
                break;
            case KC_CONTROL:
                _special->isControl = _down;
                break;
            case KC_MENU:
                _special->isAlt = _down;
                break;
            default:
                break;
            }

            _special->isSpecial = _special->isAlt == true || _special->isShift == true || _special->isControl == true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    MCPInputContext::MCPInputContext()
        : m_cursorX( 0.f )
        , m_cursorY( 0.f )
    {
        const struct MouseButtonDesc
        {
            const Char * name;
            EMouseButtonCode code;
        } mouseButtons[] = {
            {"left", MC_LBUTTON},
            {"middle", MC_MBUTTON},
            {"right", MC_RBUTTON},
            {"x1", MC_X1BUTTON},
            {"x2", MC_X2BUTTON}
        };

        for( const MouseButtonDesc & mouseButton : mouseButtons )
        {
            ConstString name = Helper::stringizeString( mouseButton.name );
            m_mouseButtons.emplace( name, mouseButton.code );
        }

        const struct KeyCodeDesc
        {
            const Char * name;
            EKeyCode code;
        } keyCodes[] = {
            {"backspace", KC_BACK}, {"tab", KC_TAB}, {"enter", KC_RETURN},
            {"return", KC_RETURN}, {"shift", KC_SHIFT}, {"control", KC_CONTROL},
            {"ctrl", KC_CONTROL}, {"alt", KC_MENU}, {"escape", KC_ESCAPE},
            {"space", KC_SPACE}, {"pageup", KC_PRIOR}, {"pagedown", KC_NEXT},
            {"end", KC_END}, {"home", KC_HOME}, {"left", KC_LEFT},
            {"up", KC_UP}, {"right", KC_RIGHT}, {"down", KC_DOWN},
            {"insert", KC_INSERT}, {"delete", KC_DELETE}, {"f1", KC_F1},
            {"f2", KC_F2}, {"f3", KC_F3}, {"f4", KC_F4},
            {"f5", KC_F5}, {"f6", KC_F6}, {"f7", KC_F7},
            {"f8", KC_F8}, {"f9", KC_F9}, {"f10", KC_F10},
            {"f11", KC_F11}, {"f12", KC_F12}
        };

        for( const KeyCodeDesc & keyCode : keyCodes )
        {
            ConstString name = Helper::stringizeString( keyCode.name );
            m_keyCodes.emplace( name, keyCode.code );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float MCPInputContext::getCursorX() const
    {
        float cursorX = m_cursorX;

        return cursorX;
    }
    //////////////////////////////////////////////////////////////////////////
    float MCPInputContext::getCursorY() const
    {
        float cursorY = m_cursorY;

        return cursorY;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputContext::setCursor( float _x, float _y )
    {
        m_cursorX = _x;
        m_cursorY = _y;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPInputContext::normalizeCoordinates( const Char * _coordinateSpace, float * const _x, float * const _y, String * const _error ) const
    {
        String coordinateSpace = _coordinateSpace;

        if( coordinateSpace != "pixels" )
        {
            return true;
        }

        const Resolution & resolution = APPLICATION_SERVICE()
            ->getCurrentWindowResolution();
        uint32_t width = resolution.getWidth();
        uint32_t height = resolution.getHeight();

        if( width == 0 )
        {
            *_error = "window resolution is zero";

            return false;
        }

        if( height == 0 )
        {
            *_error = "window resolution is zero";

            return false;
        }

        *_x /= (float)width;
        *_y /= (float)height;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPInputContext::resolvePoint( MCPSceneContext * _sceneContext, const jpp::object & _params, const Char * _coordinateSpace, mt::vec2f * const _point, String * const _error ) const
    {
        jpp::object handleObject;
        if( _params.exist( "handle", &handleObject ) == true )
        {
            if( handleObject.is_type_string() == false )
            {
                *_error = "scene handle must be a string";

                return false;
            }
        }

        const Char * handle = _params.get( "handle", "" );

        if( handle[0] != '\0' )
        {
            _sceneContext->refreshGeneration();

            NodePtr node = _sceneContext->resolveNode( handle, _error );

            if( node == nullptr )
            {
                return false;
            }

            if( _sceneContext->getNodeScreenPoint( node, _point ) == false )
            {
                *_error = "scene node has no screen position";

                return false;
            }

            return true;
        }

        float x = _params.get( "x", m_cursorX );
        float y = _params.get( "y", m_cursorY );

        if( this->normalizeCoordinates( _coordinateSpace, &x, &y, _error ) == false )
        {
            return false;
        }

        _point->x = x;
        _point->y = y;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPInputContext::getMouseButton( const Char * _button, EMouseButtonCode * const _code ) const
    {
        ConstString buttonName = Helper::stringizeString( _button );
        MapMouseButtons::const_iterator it = m_mouseButtons.find( buttonName );

        if( it == m_mouseButtons.end() )
        {
            return false;
        }

        *_code = it->second;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    EKeyCode MCPInputContext::getKeyCode( const Char * _key ) const
    {
        if( _key == nullptr )
        {
            return KC_UNASSIGNED;
        }

        if( _key[0] == '\0' )
        {
            return KC_UNASSIGNED;
        }

        if( _key[1] == '\0' )
        {
            Char ch = _key[0];

            if( ch >= 'a' && ch <= 'z' )
            {
                ch = (Char)(ch - 'a' + 'A');
            }

            if( (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') )
            {
                EKeyCode code = (EKeyCode)ch;

                return code;
            }
        }

        ConstString keyName = Helper::stringizeString( _key );
        MapKeyCodes::const_iterator it = m_keyCodes.find( keyName );

        if( it == m_keyCodes.end() )
        {
            return KC_UNASSIGNED;
        }

        EKeyCode code = it->second;

        return code;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPInputContext::getModifierKeys( const jpp::object & _params, VectorMCPKeyCodes * const _keys, String * const _error ) const
    {
        jpp::object modifiers;
        if( _params.exist( "modifiers", &modifiers ) == false )
        {
            return true;
        }

        if( modifiers.is_type_array() == false )
        {
            *_error = "modifiers must be an array of key names";

            return false;
        }

        jpp::array modifiersArray( modifiers );

        for( const jpp::object & modifier : modifiersArray )
        {
            if( modifier.is_type_string() == false )
            {
                *_error = "modifiers must be an array of key names";

                return false;
            }

            const Char * name = modifier;
            EKeyCode code = this->getKeyCode( name );

            if( code != KC_SHIFT && code != KC_CONTROL && code != KC_MENU )
            {
                *_error = "unsupported modifier '";
                _error->append( name );
                _error->append( "', expected shift, ctrl or alt" );

                return false;
            }

            VectorMCPKeyCodes::const_iterator begin = _keys->begin();
            VectorMCPKeyCodes::const_iterator end = _keys->end();
            VectorMCPKeyCodes::const_iterator it = StdAlgorithm::find( begin, end, code );

            if( it != end )
            {
                continue;
            }

            _keys->emplace_back( code );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputContext::getSpecial( const VectorMCPKeyCodes & _keys, InputSpecialData * const _special ) const
    {
        INPUT_SERVICE()
            ->getSpecial( _special );

        for( EKeyCode code : _keys )
        {
            Detail::setModifierKey( _special, code, true );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputContext::pushModifierKeys( const VectorMCPKeyCodes & _keys, bool _down ) const
    {
        Timestamp timestamp = Helper::getSystemTimestamp();

        if( _down == true )
        {
            InputSpecialData special;
            INPUT_SERVICE()
                ->getSpecial( &special );

            for( EKeyCode code : _keys )
            {
                Detail::setModifierKey( &special, code, true );

                Helper::pushKeyEvent( timestamp, m_cursorX, m_cursorY, 0.f, code, true, false, special );
            }

            return;
        }

        InputSpecialData special;
        this->getSpecial( _keys, &special );

        for( VectorMCPKeyCodes::const_reverse_iterator it = _keys.rbegin(); it != _keys.rend(); ++it )
        {
            EKeyCode code = *it;

            Detail::setModifierKey( &special, code, false );

            Helper::pushKeyEvent( timestamp, m_cursorX, m_cursorY, 0.f, code, false, false, special );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
