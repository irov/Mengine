#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/InputServiceInterface.h"

#include "Environment/SDL2/SDL2Includer.h"

#include "Kernel/Factorable.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL2Input
        : public Factorable
    {
    public:
        SDL2Input();
        ~SDL2Input() override;

    public:
        bool initialize();
        void finalize();

    public:
        void handleEvent( SDL_Window * _sdlWindow, const SDL_Event & _event );

    public:
        bool isKeyDown( EKeyCode _code ) const;
        bool isAnyKeyDown() const;

    public:
        void getCursorPosition( SDL_Window * _sdlWindow, mt::vec2f * const _point ) const;

    protected:
        void fillKeys_();

    protected:
        void calcCursorPosition_( SDL_Window * _sdlWindow, Sint32 _mx, Sint32 _my, mt::vec2f * const _point ) const;

    protected:
        ETouchCode acquireFingerIndex_( SDL_FingerID _fingerId );
        ETouchCode releaseFingerIndex_( SDL_FingerID _fingerId );
        ETouchCode getFingerIndex_( SDL_FingerID _fingerId ) const;

    protected:
        EKeyCode getKeyCode_( SDL_Scancode _key ) const;
        SDL_Scancode getSDLKey_( EKeyCode _code ) const;

    protected:
        bool m_keyDown[MENGINE_INPUT_MAX_KEY_CODE];

        EKeyCode m_keys[SDL_NUM_SCANCODES];
        SDL_Scancode m_codes[MENGINE_INPUT_MAX_KEY_CODE];

        SDL_FingerID m_fingers[MENGINE_INPUT_MAX_TOUCH];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL2Input> SDL2InputPtr;
    //////////////////////////////////////////////////////////////////////////
}
