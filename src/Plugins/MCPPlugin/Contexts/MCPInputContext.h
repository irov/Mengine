#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Map.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include "jpp/jpp.hpp"
#include "math/vec2.h"

namespace Mengine
{
    class MCPSceneContext;

    //////////////////////////////////////////////////////////////////////////
    typedef Vector<EKeyCode> VectorMCPKeyCodes;
    //////////////////////////////////////////////////////////////////////////
    class MCPInputContext
    {
    public:
        MCPInputContext();

        float getCursorX() const;
        float getCursorY() const;
        void setCursor( float _x, float _y );

        bool normalizeCoordinates( const Char * _coordinateSpace, float * const _x, float * const _y, String * const _error ) const;
        bool resolvePoint( MCPSceneContext * _sceneContext, const jpp::object & _params, const Char * _coordinateSpace, mt::vec2f * const _point, String * const _error ) const;
        bool getMouseButton( const Char * _button, EMouseButtonCode * const _code ) const;
        EKeyCode getKeyCode( const Char * _key ) const;
        bool getModifierKeys( const jpp::object & _params, VectorMCPKeyCodes * const _keys, String * const _error ) const;
        void getSpecial( const VectorMCPKeyCodes & _keys, InputSpecialData * const _special ) const;
        void pushModifierKeys( const VectorMCPKeyCodes & _keys, bool _down ) const;

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
