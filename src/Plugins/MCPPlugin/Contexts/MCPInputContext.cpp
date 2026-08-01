#include "MCPInputContext.h"

#include "Interface/ApplicationInterface.h"

#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
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

        const Resolution & resolution = APPLICATION_SERVICE()->getCurrentWindowResolution();
        uint32_t width = resolution.getWidth();
        uint32_t height = resolution.getHeight();

        if( width == 0 || height == 0 )
        {
            *_error = "window resolution is zero";

            return false;
        }

        *_x /= (float)width;
        *_y /= (float)height;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    EMouseButtonCode MCPInputContext::getMouseButton( const Char * _button ) const
    {
        ConstString buttonName = Helper::stringizeString( _button );
        MapMouseButtons::const_iterator it = m_mouseButtons.find( buttonName );

        if( it == m_mouseButtons.end() )
        {
            return MC_LBUTTON;
        }

        EMouseButtonCode button = it->second;

        return button;
    }
    //////////////////////////////////////////////////////////////////////////
    EKeyCode MCPInputContext::getKeyCode( const Char * _key ) const
    {
        if( _key == nullptr || _key[0] == '\0' )
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
}
