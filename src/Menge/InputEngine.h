#	pragma once

#	include "Interface/InputSystemInterface.h"

#	include "Core/Resolution.h"
#	include "Math/vec2.h"

#	include "Core/Holder.h"

namespace Menge
{
	class InputEngine
		: public Holder<InputEngine>
	{
	public:
		InputEngine();
		~InputEngine();

		bool initialize();
		void update();

	public:
		bool isKeyDown( KeyCode _keyCode );
		bool isModifierDown( KeyModifier _modifier );

		const mt::vec2f & getMousePosition() const;

		bool isAnyMouseButtonDown() const;
		bool isMouseButtonDown( int _button ) const;

		void setMousePosition( float _x, float _y );

		void setMouseBounded( bool _bounded );
		bool getMouseBounded() const;

		void setResolution( const Resolution & _resolution );

		void pushKeyEvent( unsigned int _key, unsigned int _char, bool _isDown );
		void pushMouseButtonEvent( int _button, bool _isDown );
		void pushMouseMoveEvent( int _x, int _y, int _z );

	private:

		enum EventType
		{
			ET_KEY = 0,
			ET_MOUSEBUTTON,
			ET_MOUSEMOVE
		};

		struct KeyEventParams
		{
			unsigned int key;
			unsigned int character;
			bool isDown;
		};

		struct MouseButtonParams
		{
			unsigned int button;
			bool isDown;
		};

		struct MouseMoveParams
		{
			int x;
			int y;
			int z;
		};

		typedef std::vector<EventType> TVectorEventType;
		typedef std::vector<KeyEventParams> TVectorKeyEventParams;
		typedef std::vector<MouseButtonParams> TVectorMouseButtonParams;
		typedef std::vector<MouseMoveParams> TVectorMouseMoveParams;

		void keyEvent( const KeyEventParams& _keyEventParams );
		void mouseButtonEvent( const MouseButtonParams& _mouseButtonParams );
		void mouseMoveEvent( const MouseMoveParams& _mouseMoveParams );

	private:
		mt::vec2f m_mousePos;
		Resolution m_resolution;
		TVectorEventType m_events;
		TVectorKeyEventParams m_keyEventParams;
		TVectorMouseButtonParams m_mouseButtonEventParams;
		TVectorMouseMoveParams m_mouseMoveEventParams;
		unsigned char m_keyBuffer[256];
		bool m_mouseBuffer[3];
	};
}
