#	pragma once

#	include <Core/Holder.h>
#	include <Interface/InputSystemInterface.h>

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
		void clear();

	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;

	protected:
		typedef std::list<GlobalMouseHandler *> TSetGlobalMouseHandler;
		TSetGlobalMouseHandler m_setGlobalMouseHandler;

		typedef std::list<GlobalKeyHandler *> TSetGlobalKeyHandler;
		TSetGlobalKeyHandler m_setGlobalKeyHandler;
	};
}