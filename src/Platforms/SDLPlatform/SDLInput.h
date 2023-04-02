#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/InputServiceInterface.h"

#include "Environment/SDL/SDLIncluder.h"

#include "Kernel/Factorable.h"

#include "math/vec2.h"

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
        bool m_keyDown[MENGINE_INPUT_MAX_KEY_CODE];

        EKeyCode m_keys[MENGINE_INPUT_MAX_KEY_CODE];
        SDL_Scancode m_codes[SDL_NUM_SCANCODES];

        SDL_FingerID m_fingers[MENGINE_INPUT_MAX_TOUCH];

    protected:
        EKeyCode getKeyCode_( SDL_Scancode _key ) const;
        SDL_Scancode getSDLKey_( EKeyCode _code ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLInput> SDLInputPtr;
    //////////////////////////////////////////////////////////////////////////
}
