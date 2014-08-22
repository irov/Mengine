#	pragma once

#	include "Interface/InputSystemInterface.h"

#	include "Core/Resolution.h"
#	include "Core/Viewport.h"

#	include "Math/vec2.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
    class NotificationServiceInterface;

	class Observer;

	class InputEngine
		: public InputServiceInterface
	{
	public:
		InputEngine();
		~InputEngine();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		void update() override;

	public:
		bool isKeyDown( size_t _keyCode ) const override;
		bool isExclusiveKeyDown( size_t _keyCode ) const override;
		bool isAnyKeyDown() const override;
		
		bool isAnyMouseButtonDown() const override;
		bool isMouseButtonDown( size_t _button ) const override;

        void setCursorPosition( const mt::vec2f & _point ) override;
		const mt::vec2f & getCursorPosition() const override;
		bool validCursorPosition( const mt::vec2f & _point ) const override;

		void addMousePositionProvider( InputMousePositionProvider * _provider ) override;
		void removeMousePositionProvider( InputMousePositionProvider * _provider ) override;

	public:
		void onFocus( bool _focus ) override;

	public:
		void onKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;

	public:
		void onMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		void onMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y ) override;
		void onMouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel ) override;

		void onMousePosition( unsigned int _touchId, const mt::vec2f & _point ) override;
        void onMouseEnter( unsigned int _touchId, const mt::vec2f & _point ) override;
        void onMouseLeave( unsigned int _touchId, const mt::vec2f & _point ) override;

	private:
		ServiceProviderInterface * m_serviceProvider;
        
		enum EventType
		{
			ET_KEY = 0,
			ET_MOUSEBUTTON,
			ET_MOUSEMOVE,
			ET_MOUSEWHEEL,
			ET_MOUSEPOSITION,
            ET_MOUSEENTER,
            ET_MOUSELEAVE
		};

		struct KeyEventParams
		{
			mt::vec2f point;

			unsigned int key;
			unsigned int character;
			bool isDown;
		};

		struct MouseButtonParams
		{
			unsigned int touchId;
			mt::vec2f point;

			unsigned int button;
			bool isDown;
		};

		struct MouseMoveParams
		{
			unsigned int touchId;
			mt::vec2f point;

			float x;
			float y;
			int wheel;
		};

		struct MousePositionParams
		{
			unsigned int touchId;
			mt::vec2f point;
		};

		typedef stdex::vector<EventType> TVectorEventType;
		typedef stdex::vector<KeyEventParams> TVectorKeyEventParams;
		typedef stdex::vector<MouseButtonParams> TVectorMouseButtonParams;
		typedef stdex::vector<MouseMoveParams> TVectorMouseMoveParams;
		typedef stdex::vector<MousePositionParams> TVectorMousePositionParams;

    protected:
		void keyEvent_( const KeyEventParams& _params );
		void mouseButtonEvent_( const MouseButtonParams& _params );
		void mouseMoveEvent_( const MouseMoveParams& _params );
		void mouseWheelEvent_( const MouseMoveParams& _params );
		void mousePositionEvent_( const MousePositionParams& _params );
        void mouseEnterEvent_( const MousePositionParams& _params );
        void mouseLeaveEvent_( const MousePositionParams& _params );

	protected:		
		void applyCursorPosition_( const mt::vec2f & _point );

	private:
		mt::vec2f m_cursorPosition;

		typedef stdex::vector<InputMousePositionProvider *> TVectorMousePositionProviders;
		TVectorMousePositionProviders m_mousePositionProviders;
		
        TVectorEventType m_eventsAdd;
		TVectorEventType m_events;
		TVectorKeyEventParams m_keyEventParams;
		TVectorMouseButtonParams m_mouseButtonEventParams;
		TVectorMouseMoveParams m_mouseMoveEventParams;
		TVectorMousePositionParams m_mousePositionEventParams;

		bool m_keyBuffer[256];
		bool m_mouseBuffer[3];
	};
}
