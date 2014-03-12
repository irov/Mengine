#	pragma once

#   include "Interface/MousePickerSystemInterface.h"

#	include "Core/Viewport.h"

#	include <list>
#	include <vector>

namespace Menge
{	
	struct PickerTrapStateDesc
	{
		PickerTrapState * state;
		const RenderViewportInterface * viewport;
		const RenderCameraInterface * camera;
	};

	typedef std::vector<PickerTrapStateDesc> TVectorPickerTrapStates;
	
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
		void setRenderViewport( const RenderViewportInterface * _viewport ) override;
		void setRenderCamera( const RenderCameraInterface * _camera ) override;

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
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating ) override;
		bool handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell ) override;
				
	public:
		void handleMouseEnter( const mt::vec2f & _point ) override;
		void handleMouseLeave() override;

	public:
		size_t getPickerTrapCount() const override;

	private:
		bool proccesTraps_( const mt::vec2f & _point, TVectorPickerTrapStates & _states );
		void updateDead_();

	private:
        ServiceProviderInterface * m_serviceProvider;

		size_t m_enumerator;

		bool m_block;
        bool m_handleValue;

		Arrow * m_arrow;
		Scene * m_scene;

		const RenderViewportInterface * m_viewport;
		const RenderCameraInterface * m_camera;

		typedef std::list<PickerTrapState> TPickerTrapState;
		TPickerTrapState m_pickerTrapState;

		TVectorPickerTrapStates m_states;

		size_t m_pickerTrapCount;
	};
}
