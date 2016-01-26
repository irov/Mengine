#	pragma once

#   include "Interface/MousePickerSystemInterface.h"

#	include "Core/Viewport.h"

#	include <stdex/stl_list.h>
#	include <stdex/stl_vector.h>

namespace Menge
{	
	struct PickerTrapStateDesc
	{
		PickerTrapState * state;
		const RenderViewportInterface * viewport;
		const RenderCameraInterface * camera;
	};

	typedef stdex::vector<PickerTrapStateDesc> TVectorPickerTrapStates;
	
	class MousePickerSystem
		: public MousePickerSystemInterface
	{
	public:
		MousePickerSystem( ServiceProviderInterface * _serviceProvider );

	public:
		void setBlock( bool _value ) override;
        void setHandleValue( bool _value ) override;

	public:
		void setArrow( Arrow * _arrow ) override;
		void setScene( Scene * _scene ) override;
		void setGameport( const Viewport & _gameport ) override;
		void setRenderViewport( const RenderViewportInterface * _viewport ) override;
		void setRenderCamera( const RenderCameraInterface * _camera ) override;
		void setRenderClipplane( const RenderClipplaneInterface * _clipplane ) override;

	public:
		void update() override;
		void clear() override;

    public:
		bool pickTrap( const mt::vec2f& _point, TVectorPickerTraps & _traps ) override;
		
	public:
		PickerTrapState * regTrap( MousePickerTrapInterface * _trap ) override;		
		void unregTrap( PickerTrapState * _id ) override;

		void updateTrap() override;

	public:
		bool handleKeyEvent( const InputKeyEvent & _event ) override;

	public:
		bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
		bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
		bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
		bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
		bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;
				
	public:
		void handleMouseEnter( const InputMousePositionEvent & _event ) override;
		void handleMouseLeave( const InputMousePositionEvent & _event ) override;

	public:
		uint32_t getPickerTrapCount() const override;

	private:
		bool proccesTraps_( float _x, float _y, TVectorPickerTrapStates & _states );
		void updateDead_();

	private:
        ServiceProviderInterface * m_serviceProvider;

		uint32_t m_enumerator;

		bool m_block;
        bool m_handleValue;

		Arrow * m_arrow;
		Scene * m_scene;
		Viewport m_gameport;

		const RenderViewportInterface * m_viewport;
		const RenderCameraInterface * m_camera;
		const RenderClipplaneInterface * m_clipplane;

		typedef stdex::list<PickerTrapState> TPickerTrapState;
		TPickerTrapState m_pickerTrapState;

		TVectorPickerTrapStates m_states;

		uint32_t m_pickerTrapCount;
	};
}
