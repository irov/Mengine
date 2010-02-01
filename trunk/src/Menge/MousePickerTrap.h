#	pragma once

namespace Menge
{
	class Arrow;
	class InputHandler;

	class MousePickerTrap
	{
	public:
		virtual bool pick( Arrow * _arrow ) = 0;
		virtual InputHandler * getInputHandler() = 0;

	public:
		virtual bool onEnter() = 0;
		virtual void onLeave() = 0;

	public:
		virtual bool _pickerActive() const = 0;
	};
}
