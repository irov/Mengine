#pragma once

#include "Interface/InputSystemInterface.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/Resolution.h"
#include "Kernel/Viewport.h"

#include "math/vec2.h"

#include "Config/Vector.h"

namespace Mengine
{
	class InputEngine
		: public ServiceBase<InputServiceInterface>
	{
	public:
		InputEngine();
		~InputEngine() override;

	public:
		bool _initializeService() override;
		void _finalizeService() override;

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
        bool validCursorPosition( float & _x, float & _y ) const override;

        uint32_t addMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider ) override;
        void removeMousePositionProvider( uint32_t _id ) override;

	public:
		void onFocus( bool _focus ) override;

	public:
		void pushEvent( const InputUnionEvent & _event ) override;
       
    protected:
		void keyEvent_( const InputKeyEvent & _params );
		void textEvent_( const InputTextEvent & _params );
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

        struct InputMousePositionProviderDesc
        {
            uint32_t id;
            InputMousePositionProviderInterfacePtr provider;
        };

        uint32_t m_enumerator;

		typedef Vector<InputMousePositionProviderDesc> VectorMousePositionProviders;
		VectorMousePositionProviders m_mousePositionProviders;
				
		typedef Vector<InputUnionEvent> VectorInputEvents;
		VectorInputEvents m_eventsAdd;
		VectorInputEvents m_events;

		bool m_keyBuffer[256];
		bool m_mouseBuffer[3];

        class FMousePositionProviderFind;
	};
}
