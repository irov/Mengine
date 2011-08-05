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
		virtual bool pick( const mt::vec2f& _point, Arrow * _arrow ) = 0;

	public:
		virtual bool onMouseEnter() = 0;
		virtual void onMouseLeave() = 0;

	public:
		virtual bool _pickerActive() const = 0;
	};
}
