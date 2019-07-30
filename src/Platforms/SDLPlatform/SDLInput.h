#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/InputServiceInterface.h"

#include "Kernel/Factorable.h"

#include "math/vec2.h"
#include "stdex/stl_map.h"

#include "SDL.h"

namespace Mengine
{
    class SDLInput
        : public Factorable
    {
    public:
        SDLInput();
        ~SDLInput() override;

    public:
        bool initialize();
        void finalize();

    public:
        void handleEvent( const SDL_Event & _event );

    public:
        void updateSurfaceResolution( float _width, float _height );

    public:
        void showKeyboard( bool _value );

    public:
        bool isKeyDown( EKeyCode _code ) const;
        bool isAnyKeyDown() const;

    protected:
        void fillKeys_();

    protected:
        void calcCursorPosition_( int _mx, int _my, mt::vec2f & _point ) const;

    protected:
        uint32_t acquireFingerIndex_( SDL_FingerID _fingerId );
        uint32_t releaseFingerIndex_( SDL_FingerID _fingerId );
        uint32_t getFingerIndex_( SDL_FingerID _fingerId ) const;

    protected:
        bool m_keyDown[KC_MAX_CODE];

        EKeyCode m_keys[KC_MAX_CODE];
        SDL_Scancode m_codes[SDL_NUM_SCANCODES];

        SDL_FingerID m_fingers[MENGINE_INPUT_MAX_TOUCH];

        float m_width;
        float m_height;

    protected:
        EKeyCode getKeyCode_( SDL_Scancode _key ) const;
        SDL_Scancode getSDLKey_( EKeyCode _code ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLInput> SDLInputPtr;
    //////////////////////////////////////////////////////////////////////////
}
