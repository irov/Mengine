#	pragma once

#	include <list>

namespace Menge
{
	class MousePickerTrap;
	class InputHandler;

	class MousePickerSystem
	{
	public:
		MousePickerSystem();

	public:
		void update();
		void clear();

		void regTrap( MousePickerTrap * _trap );
		MousePickerTrap * pickTrap( float _x, float _y );
		InputHandler * pickHandler( float _x, float _y );

		bool handleKeyEvent( size_t _key, bool _isDown );
		bool handleMouseButtonEvent( size_t _button, bool _isDown );
		bool handleMouseMove( float _x, float _y, float _whell );

	private:
		typedef std::list<MousePickerTrap *> TListPickerTrap;
		TListPickerTrap m_listPickerTrap;

		MousePickerTrap * m_lastPickerTrap;
	};
}