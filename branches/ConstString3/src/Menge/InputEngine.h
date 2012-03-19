#	pragma once

#	include "Interface/InputSystemInterface.h"

#	include "Core/Resolution.h"
#	include "Core/Viewport.h"

#	include "Math/vec2.h"

#	include "Core/Holder.h"

namespace Menge
{
	class CursorPositionProvider
	{
	public:
		virtual void onCursorPositionChange( const mt::vec2f & _position ) = 0;
	};

	class InputEngine
		: public Holder<InputEngine>
	{
	public:
		InputEngine();
		~InputEngine();

		bool initialize();
		void update();

	public:
		void setDimentions( const Resolution & _contentResolution, const Viewport & _viewport );

	public:
		bool isKeyDown( KeyCode _keyCode );
		bool isModifierDown( KeyModifier _modifier );
		
		bool isAnyMouseButtonDown() const;
		bool isMouseButtonDown( int _button ) const;

		void setCursorPosition( const mt::vec2f & _point );
		const mt::vec2f & getCursorPosition() const;
		bool validCursorPosition( const mt::vec2f & _point ) const;

		bool isFocus() const;

		void setMouseBounded( bool _bounded );
		bool getMouseBounded() const;

		void pushKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown );
		void pushMouseButtonEvent( const mt::vec2f & _point, int _button, bool _isDown );
		void pushMouseMoveEvent( const mt::vec2f & _point, int _x, int _y, int _z );

		void addCursorPositionProvider( CursorPositionProvider * _provider );
		void removeCursorPositionProvider( CursorPositionProvider * _provider );

	private:
		enum EventType
		{
			ET_KEY = 0,
			ET_MOUSEBUTTON,
			ET_MOUSEMOVE
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
			mt::vec2f point;

			unsigned int button;
			bool isDown;
		};

		struct MouseMoveParams
		{
			mt::vec2f point;

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

	protected:		
		void applyCursorPosition_( const mt::vec2f & _point, mt::vec2f & _local ) const;

	private:
		mt::vec2f m_cursorPosition;
		mt::vec2f m_dimentions;

		typedef std::vector<CursorPositionProvider*> TVectorCursorPositionProviders;
		TVectorCursorPositionProviders m_cursorPositionProviders;
		
		TVectorEventType m_events;
		TVectorKeyEventParams m_keyEventParams;
		TVectorMouseButtonParams m_mouseButtonEventParams;
		TVectorMouseMoveParams m_mouseMoveEventParams;
		unsigned char m_keyBuffer[256];
		bool m_mouseBuffer[3];

		mt::vec2f m_inputScale;
		mt::vec2f m_inputOffset;		
	};
}
