#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/InputSystemInterface.h"

#   include "s3ePointer.h"
#   include "s3eKeyboard.h"

#   include "math/vec2.h"

#   define MENGINE_MAX_TOUCHES 2

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

    public:
        bool update();

    public:
        void showKeyboard( bool _value ); 

    public:
        bool isKeyDown( Menge::KeyCode _code ) const;
        bool isAnyKeyDown() const;

    protected:
        void fillKeys_();
        KeyCode getKeyCode_( s3eKey _key ) const;
        s3eKey gets3eKey_( KeyCode _code ) const;

    protected:
        static void s_keyboardKeyEvent( s3eKeyboardEvent * _event, MarmaladeInput * _input );
        static void s_keyboardCharEvent( s3eKeyboardCharEvent * _event, MarmaladeInput * _input );
        static void s_pointerTouchEvent( s3ePointerTouchEvent * _event, MarmaladeInput * _input );
        static void s_pointerTouchMotionEvent( s3ePointerTouchMotionEvent * _event, MarmaladeInput * _input );
        static void s_pointerButtonEvent( s3ePointerEvent * _event, MarmaladeInput * _input );
        static void s_pointerMotionEvent( s3ePointerMotionEvent * _event, MarmaladeInput * _input );
		
	protected:
		void correctPoint_( int32 _x, int32 _y, mt::vec2f & _point ) const;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        struct TouchDesc
        {
            mt::vec2f point;
            bool active;
        };

        TouchDesc m_touches[MENGINE_MAX_TOUCHES];
        
        KeyCode m_keys[255];
        s3eKey m_codes[255];

        bool m_showKeyboard;

		float m_width;
		float m_height;

    protected:
        bool setTouch_( uint32 _touchId, const mt::vec2f & _point, bool _active );
        bool getTouch_( uint32 _touchId, TouchDesc & _desc );
    };
}