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
        bool isKeyDown( Mengine::KeyCode _code ) const;
        bool isAnyKeyDown() const;

    protected:
        void fillKeys_();
        
    protected:
        void calcCursorPosition_( int _mx, int _my, mt::vec2f & _point ) const;

    protected:
        bool m_keyDown[KC_MAX_CODE];
       
        KeyCode m_keys[KC_MAX_CODE];
        SDL_Scancode m_codes[SDL_NUM_SCANCODES];

        float m_width;
        float m_height;

    protected:
        KeyCode getKeyCode_( SDL_Scancode _key ) const;
        SDL_Scancode getSDLKey_( KeyCode _code ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<SDLInput> SDLInputPtr;
    //////////////////////////////////////////////////////////////////////////
}
