#	pragma once

#	include "InputHandler.h"
#	include "Scriptable.h"

namespace Menge
{
	class Arrow;

	class MousePickerTrap
		: virtual public InputHandler
		, virtual public Scriptable
	{
	public:
		virtual bool pick( Arrow * _arrow ) = 0;

	public:
		virtual bool onEnter() = 0;
		virtual void onLeave() = 0;

	public:
		virtual bool _pickerActive() const = 0;
	};
}
