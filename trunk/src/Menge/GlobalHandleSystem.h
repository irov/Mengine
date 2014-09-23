#	pragma once

#	include "Interface/GlobalHandleSystemInterface.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
    struct GlobalMouseHandleDesc
    {
        uint32_t id;

        GlobalMouseHandler * handler;
        bool dead;
    };

    struct GlobalKeyHandleDesc
    {
        uint32_t id;

        GlobalKeyHandler * handler;
        bool dead;
    };

	class GlobalHandleSystem
		: public GlobalHandleSystemInterface
	{
    public:
        GlobalHandleSystem( ServiceProviderInterface * _serviceProvider );

	public:
		uint32_t addGlobalMouseEventable( GlobalMouseHandler * _handler ) override;
		GlobalMouseHandler * removeGlobalMouseEventable( uint32_t _id ) override;

		uint32_t addGlobalKeyEventable( GlobalKeyHandler * _handler ) override;
		GlobalKeyHandler * removeGlobalKeyEventable( uint32_t _id ) override;

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
		void handleGlobalMouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel ) override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        		
		typedef stdex::vector<GlobalMouseHandleDesc> TVectorGlobalMouseHandler;
		TVectorGlobalMouseHandler m_globalMouseHandler;
        TVectorGlobalMouseHandler m_globalMouseHandlerAdd;

		typedef stdex::vector<GlobalKeyHandleDesc> TVectorGlobalKeyHandler;
		TVectorGlobalKeyHandler m_globalKeyHandler;

        uint32_t m_handlersEnumerator;
	};
}
