#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/InputSystemInterface.h"

#   include "math/vec2.h"

#	include "stdex/stl_map.h"

#	include "SDL.h"

namespace Menge
{
    class SDLInput
    {
    public:
		SDLInput();
		~SDLInput();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );
        ServiceProviderInterface * getServiceProvider();

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
        bool isKeyDown( Menge::KeyCode _code ) const;
        bool isAnyKeyDown() const;

    protected:
        void fillKeys_();
		
	protected:
		void calcCursorPosition_( int _mx, int _my, mt::vec2f & _point ) const;

    protected:
        ServiceProviderInterface * m_serviceProvider;

		bool m_keyDown[KC_MAX_CODE];
       
		KeyCode m_keys[KC_MAX_CODE];
		SDL_Scancode m_codes[SDL_NUM_SCANCODES];

		float m_width;
		float m_height;

    protected:
		KeyCode getKeyCode_( SDL_Scancode _key ) const;
		SDL_Scancode getSDLKey_( KeyCode _code ) const;
    };
}