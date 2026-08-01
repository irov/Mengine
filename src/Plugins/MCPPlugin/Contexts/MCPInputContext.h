#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Kernel/Map.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPInputContext
    {
    public:
        MCPInputContext();

        float getCursorX() const;
        float getCursorY() const;
        void setCursor( float _x, float _y );

        bool normalizeCoordinates( const Char * _coordinateSpace, float * const _x, float * const _y, String * const _error ) const;
        EMouseButtonCode getMouseButton( const Char * _button ) const;
        EKeyCode getKeyCode( const Char * _key ) const;

    protected:
        typedef Map<ConstString, EMouseButtonCode> MapMouseButtons;
        typedef Map<ConstString, EKeyCode> MapKeyCodes;

        float m_cursorX;
        float m_cursorY;
        MapMouseButtons m_mouseButtons;
        MapKeyCodes m_keyCodes;
    };
    //////////////////////////////////////////////////////////////////////////
}
