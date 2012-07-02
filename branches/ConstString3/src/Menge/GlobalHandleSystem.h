#	pragma once

#	include "Core/Holder.h"
#	include "Interface/InputSystemInterface.h"

#	include <list>

namespace Menge
{
	class GlobalMouseHandler;
	class GlobalKeyHandler;

	class GlobalHandleSystem
		: public Holder<GlobalHandleSystem>
		, public InputSystemHandler
	{
	public:
		void regGlobalMouseEventable( GlobalMouseHandler * _handler );
		void unregGlobalMouseEventable( GlobalMouseHandler * _handler );

		void regGlobalKeyEventable( GlobalKeyHandler * _handler );
		void unregGlobalKeyEventable( GlobalKeyHandler * _handler );

	public:
		void update();
		void clear();

	public:
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;

	public:
		bool handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell ) override;

	protected:
		typedef std::list<GlobalMouseHandler *> TSetGlobalMouseHandler;
		TSetGlobalMouseHandler m_globalMouseHandler;
		TSetGlobalMouseHandler m_addGlobalMouseHandler;
		TSetGlobalMouseHandler m_delGlobalMouseHandler;

		typedef std::list<GlobalKeyHandler *> TSetGlobalKeyHandler;
		TSetGlobalKeyHandler m_globalKeyHandler;
		TSetGlobalKeyHandler m_addGlobalKeyHandler;
		TSetGlobalKeyHandler m_delGlobalKeyHandler;
	};
}
