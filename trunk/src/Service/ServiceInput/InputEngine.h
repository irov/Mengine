#	pragma once

#	include "Interface/InputSystemInterface.h"

#	include "Core/Resolution.h"
#	include "Core/Viewport.h"

#	include "Math/vec2.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
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
		bool isAltDown() const override;
		bool isShiftDown() const override;
		bool isCtrlDown() const override;

	public:
		bool isKeyDown( uint32_t _keyCode ) const override;
		bool isExclusiveKeyDown( uint32_t _keyCode ) const override;
		bool isAnyKeyDown() const override;
		
		bool isAnyMouseButtonDown() const override;
		bool isMouseButtonDown( uint32_t _button ) const override;

		void setCursorPosition( uint32_t _touchId, const mt::vec2f & _point ) override;
		const mt::vec2f & getCursorPosition( uint32_t _touchId ) const override;
		bool validCursorPosition( float _x, float _y ) const override;

		void addMousePositionProvider( InputMousePositionProvider * _provider ) override;
		void removeMousePositionProvider( InputMousePositionProvider * _provider ) override;

	public:
		void onFocus( bool _focus ) override;

	public:
		void pushEvent( const InputUnionEvent & _event ) override;

	private:
		ServiceProviderInterface * m_serviceProvider;
        
    protected:
		void keyEvent_( const InputKeyEvent & _params );
		void mouseButtonEvent_( const InputMouseButtonEvent& _params );
		void mouseMoveEvent_( const InputMouseMoveEvent& _params );
		void mouseWheelEvent_( const InputMouseWheelEvent & _params );
		void mousePositionEvent_( const InputMousePositionEvent & _params );
		void mouseEnterEvent_( const InputMousePositionEvent & _params );
		void mouseLeaveEvent_( const InputMousePositionEvent & _params );

	protected:		
		void applyCursorPosition_( uint32_t _touchId, float _x, float _y );

	private:
		mt::vec2f m_cursorPosition[MENGINE_INPUT_MAX_TOUCH];

		typedef stdex::vector<InputMousePositionProvider *> TVectorMousePositionProviders;
		TVectorMousePositionProviders m_mousePositionProviders;
				
		typedef stdex::vector<InputUnionEvent> TVectorInputEvent;
		TVectorInputEvent m_eventsAdd;
		TVectorInputEvent m_events;

		bool m_keyBuffer[256];
		bool m_mouseBuffer[3];
	};
}
