#	pragma once

#	include "Interface/GlobalHandleSystemInterface.h"

#	include <vector>

namespace Menge
{
    struct GlobalMouseHandleDesc
    {
        size_t id;

        GlobalMouseHandler * handler;
        bool dead;
    };

    struct GlobalKeyHandleDesc
    {
        size_t id;

        GlobalKeyHandler * handler;
        bool dead;
    };

	class GlobalHandleSystem
		: public GlobalHandleSystemInterface
	{
    public:
        GlobalHandleSystem( ServiceProviderInterface * _serviceProvider );

	public:
		size_t addGlobalMouseEventable( GlobalMouseHandler * _handler ) override;
		GlobalMouseHandler * removeGlobalMouseEventable( size_t _id ) override;

		size_t addGlobalKeyEventable( GlobalKeyHandler * _handler ) override;
		GlobalKeyHandler * removeGlobalKeyEventable( size_t _id ) override;

	public:
		void update() override;
        void clear() override;

	public:
		void handleGlobalKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating ) override;

	public:
		void handleGlobalMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		void handleGlobalMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		void handleGlobalMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		void handleGlobalMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y ) override;
		void handleGlobalMouseWhell( unsigned int _touchId, const mt::vec2f & _point, int _whell ) override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        		
		typedef std::vector<GlobalMouseHandleDesc> TVectorGlobalMouseHandler;
		TVectorGlobalMouseHandler m_globalMouseHandler;
        TVectorGlobalMouseHandler m_globalMouseHandlerAdd;

		typedef std::vector<GlobalKeyHandleDesc> TVectorGlobalKeyHandler;
		TVectorGlobalKeyHandler m_globalKeyHandler;

        size_t m_handlersEnumerator;
	};
}
