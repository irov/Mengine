#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/InputSystemInterface.h"

#   include "s3ePointer.h"
#   include "s3eKeyboard.h"

#   include "math/vec2.h"

#	include "stdex/stl_map.h"

#   define MENGINE_MAX_TOUCHES 2
#   define MENGINE_MAX_KEYS 64

namespace Menge
{
    class MarmaladeInput
    {
    public:
        MarmaladeInput();
        ~MarmaladeInput();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );
        ServiceProviderInterface * getServiceProvider();

    public:
        bool initialize();
		void finalize();

    public:
        bool update();

    public:
        void updateSurfaceResolution();

    public:
        void showKeyboard( bool _value ); 

    public:
        bool isKeyDown( Menge::KeyCode _code ) const;
        bool isAnyKeyDown() const;

	public:
		void getCorrectPoint( uint32 _touch, mt::vec2f & _point ) const;

    protected:
        void fillKeys_();
        KeyCode getKeyCode_( s3eKey _key ) const;
        s3eKey gets3eKey_( KeyCode _code ) const;

    protected:
		static int32 s_keyboardKeyEvent( void * _event, void * _input );
		static int32 s_keyboardCharEvent( void * _event, void * _input );
		static int32 s_pointerTouchEvent( void * _event, void * _input );
		static int32 s_pointerTouchMotionEvent( void * _event, void * _input );
		static int32 s_pointerButtonEvent( void * _event, void * _input );
		static int32 s_pointerMotionEvent( void * _event, void * _input );

	protected:
		int32 keyboardKeyEvent( s3eKeyboardEvent * _event );
		int32 keyboardCharEvent( s3eKeyboardCharEvent * _event );
		int32 pointerTouchEvent( s3ePointerTouchEvent * _event );
		int32 pointerTouchMotionEvent( s3ePointerTouchMotionEvent * _event );
		int32 pointerButtonEvent( s3ePointerEvent * _event );
		int32 pointerMotionEvent( s3ePointerMotionEvent * _event );
		
	protected:
		void correctPoint_( int32 _x, int32 _y, mt::vec2f & _point ) const;

    protected:
        ServiceProviderInterface * m_serviceProvider;

		struct KeyEventDesc
		{
			s3eKey key;
			uint32 pressed;
			s3eWChar ch;
		};

		KeyEventDesc m_keyEvents[MENGINE_MAX_KEYS];
		uint32_t m_keysIterator;
		s3eKey m_lastKey;

        struct TouchDesc
        {
            mt::vec2f point;
            bool active;
        };

        TouchDesc m_touches[MENGINE_MAX_TOUCHES];
        
        KeyCode m_keys[255];
        s3eKey m_codes[255];

		typedef stdex::map<s3eWChar, KeyCode> TMapWCharCode;
		TMapWCharCode m_wcharCodes;
		
		float m_width;
		float m_height;

		bool m_showKeyboard;
		bool m_alwaysKeyboard;

    protected:
        bool setTouch_( uint32 _touchId, const mt::vec2f & _point, bool _active );
        bool getTouch_( uint32 _touchId, TouchDesc & _desc );
    };
}