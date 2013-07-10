#	pragma once

#	include "Interface/InputSystemInterface.h"

#	include "Core/Resolution.h"
#	include "Core/Viewport.h"

#	include "Math/vec2.h"

#   include <vector>

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
		bool isKeyDown( KeyCode _keyCode ) override;
		bool isModifierDown( KeyModifier _modifier ) override;
		
		bool isAnyMouseButtonDown() const override;
		bool isMouseButtonDown( int _button ) const override;

        void setCursorPosition( const mt::vec2f & _point ) override;
		const mt::vec2f & getCursorPosition() const override;
		bool validCursorPosition( const mt::vec2f & _point ) const override;

		void addMousePositionProvider( InputMousePositionProvider * _provider ) override;
		void removeMousePositionProvider( InputMousePositionProvider * _provider ) override;

	public:
		void onKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;

	public:
		void onMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, int _button, bool _isDown ) override;
		void onMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell ) override;

		void onMousePosition( unsigned int _touchId, const mt::vec2f & _point ) override;
        void onMouseEnter( unsigned int _touchId, const mt::vec2f & _point ) override;
        void onMouseLeave( unsigned int _touchId, const mt::vec2f & _point ) override;


	protected:
		void notifyChangeWindowResolution(  bool _fullscreen, Resolution _resolution );

	private:
		ServiceProviderInterface * m_serviceProvider;
        
		Observer * m_notifyChangeWindowResolution;

		enum EventType
		{
			ET_KEY = 0,
			ET_MOUSEBUTTON,
			ET_MOUSEMOVE,
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
			int whell;
		};

		struct MousePositionParams
		{
			unsigned int touchId;
			mt::vec2f point;
		};

		typedef std::vector<EventType> TVectorEventType;
		typedef std::vector<KeyEventParams> TVectorKeyEventParams;
		typedef std::vector<MouseButtonParams> TVectorMouseButtonParams;
		typedef std::vector<MouseMoveParams> TVectorMouseMoveParams;
		typedef std::vector<MousePositionParams> TVectorMousePositionParams;

    protected:
		void keyEvent( const KeyEventParams& _params );
		void mouseButtonEvent( const MouseButtonParams& _params );
		void mouseMoveEvent( const MouseMoveParams& _params );
		void mousePositionEvent( const MousePositionParams& _params );
        void mouseEnterEvent( const MousePositionParams& _params );
        void mouseLeaveEvent( const MousePositionParams& _params );

	protected:		
		void applyCursorPosition_( const mt::vec2f & _point, mt::vec2f & _local );

	private:
		mt::vec2f m_cursorPosition;
		mt::vec2f m_dimentions;

		typedef std::vector<InputMousePositionProvider*> TVectorMousePositionProviders;
		TVectorMousePositionProviders m_mousePositionProviders;
		
        TVectorEventType m_eventsAdd;
		TVectorEventType m_events;
		TVectorKeyEventParams m_keyEventParams;
		TVectorMouseButtonParams m_mouseButtonEventParams;
		TVectorMouseMoveParams m_mouseMoveEventParams;
		TVectorMousePositionParams m_mousePositionEventParams;

		unsigned char m_keyBuffer[256];
		bool m_mouseBuffer[3];

		mt::vec2f m_inputScale;
        mt::vec2f m_inputOffset;

        Viewport m_inputViewport;		
	};
}
