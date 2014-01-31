#	include "InputEngine.h"
#	include "Application.h"

#	include "Interface/NotificationServiceInterace.h"
#	include "Interface/NotificatorInterface.h"

#   include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( InputService, Menge::InputServiceInterface, Menge::InputEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	InputEngine::InputEngine()
		: m_serviceProvider(nullptr)
		, m_cursorPosition(0.f, 0.f)
        , m_dimentions(0.f, 0.f)
		, m_inputScale(1.f, 1.f)
        , m_inputOffset(0.f, 0.f)
        , m_inputViewport(0.f, 0.f, 0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	InputEngine::~InputEngine()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void InputEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * InputEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::initialize()
	{
		m_notifyChangeWindowResolution = NOTIFICATION_SERVICE(m_serviceProvider)
            ->addObserverMethod( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, this, &InputEngine::notifyChangeWindowResolution );

		std::fill( m_keyBuffer, m_keyBuffer + sizeof(m_keyBuffer), false );

		m_mouseBuffer[0] = false;
		m_mouseBuffer[1] = false;
		m_mouseBuffer[2] = false;

        m_eventsAdd.reserve(16);
        m_events.reserve(16);
        m_keyEventParams.reserve(16);
        m_mouseButtonEventParams.reserve(16);
        m_mouseMoveEventParams.reserve(16);
        m_mousePositionEventParams.reserve(16);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::finalize()
	{
		NOTIFICATION_SERVICE(m_serviceProvider)
			->removeObserver( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, m_notifyChangeWindowResolution );

		m_notifyChangeWindowResolution = 0;

        m_events.clear();
        m_eventsAdd.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::update()
	{
		TVectorKeyEventParams::iterator it_keyParams = m_keyEventParams.begin();
		TVectorMouseButtonParams::iterator it_mouseButtonParams = m_mouseButtonEventParams.begin();
		TVectorMouseMoveParams::iterator it_mouseMoveParams = m_mouseMoveEventParams.begin();
		TVectorMousePositionParams::iterator it_mousePositionParams = m_mousePositionEventParams.begin();

        m_events = m_eventsAdd;
        
		for( TVectorEventType::const_iterator 
			it = m_events.begin(), 
			it_end = m_events.end();
		it != it_end;
		++it )
		{
			const EventType & eventType = (*it);

            switch( eventType )
            {
            case ET_KEY:
                {
                    const KeyEventParams& params = (*it_keyParams);
                    this->keyEvent_( params );
                    ++it_keyParams;
                }break;
            case ET_MOUSEBUTTON:
                {
                    const MouseButtonParams& params = (*it_mouseButtonParams);
                    this->mouseButtonEvent_( params );
                    ++it_mouseButtonParams;
                }break;
            case ET_MOUSEMOVE:
                {
                    const MouseMoveParams& params = (*it_mouseMoveParams);
                    this->mouseMoveEvent_( params );
                    ++it_mouseMoveParams;
                }break;
            case ET_MOUSEPOSITION:
                {
                    const MousePositionParams & params = (*it_mousePositionParams);
                    this->mousePositionEvent_( params );
                    ++it_mousePositionParams;
                }break;
            case ET_MOUSEENTER:
                {
                    const MousePositionParams & params = (*it_mousePositionParams);
                    this->mouseEnterEvent_(params);
                    ++it_mousePositionParams;
                }break;
            case ET_MOUSELEAVE:
                {
                    const MousePositionParams & params = (*it_mousePositionParams);
                    this->mouseLeaveEvent_(params);
                    ++it_mousePositionParams;
                }
            }
		}

		m_events.clear();
        m_eventsAdd.clear();

		m_keyEventParams.clear();
		m_mouseButtonEventParams.clear();
		m_mouseMoveEventParams.clear();
        m_mousePositionEventParams.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isKeyDown( size_t _keyCode ) const
	{
		bool isDown = m_keyBuffer[_keyCode];

		return isDown;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isExclusiveKeyDown( size_t _keyCode ) const
	{
		bool isDown = m_keyBuffer[_keyCode];

		if( isDown == false )
		{
			return false;
		}

		size_t keyDownCount = std::count( m_keyBuffer, m_keyBuffer + 256, true );
		
		if( keyDownCount != 1 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isAnyMouseButtonDown() const
	{
		return m_mouseBuffer[0] || m_mouseBuffer[1] || m_mouseBuffer[2];
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isMouseButtonDown( size_t _button ) const
	{
		return m_mouseBuffer[_button];
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::validCursorPosition( const mt::vec2f & _point ) const
	{
		if( _point.x < 0.f || _point.x > m_dimentions.x )
		{
			return false;
		}

		if( _point.y < 0.f || _point.y > m_dimentions.y )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::applyCursorPosition_( const mt::vec2f & _point, mt::vec2f & _local )
	{		       
        m_cursorPosition.x = m_inputOffset.x + (_point.x - m_inputViewport.begin.x) * m_inputScale.x;
        m_cursorPosition.y = m_inputOffset.y + (_point.y - m_inputViewport.begin.y) * m_inputScale.y;

		for( TVectorMousePositionProviders::iterator
			it = m_mousePositionProviders.begin(),
			it_end = m_mousePositionProviders.end();
		it != it_end;
		++it )
		{
			(*it)->onMousePositionChange( m_cursorPosition );
		}

		_local = m_cursorPosition;
	}
    //////////////////////////////////////////////////////////////////////////
    void InputEngine::setCursorPosition( const mt::vec2f & _point )
    {
        mt::vec2f point;
        this->applyCursorPosition_( _point, point );
    }
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & InputEngine::getCursorPosition() const
	{
		return m_cursorPosition;
	}
    //////////////////////////////////////////////////////////////////////////
    void InputEngine::calcCursorUnviewport( const mt::vec2f & _point, mt::vec2f & _result ) const
    {
        mt::vec2f cp;
            
        cp.x = _point.x - m_inputOffset.x;
        cp.y = _point.y - m_inputOffset.y;

        cp.x /= m_inputScale.x;
        cp.y /= m_inputScale.y;

        cp.x += m_inputViewport.begin.x;
        cp.y += m_inputViewport.begin.y;

        _result = cp;
    }
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::addMousePositionProvider( InputMousePositionProvider * _provider )
	{
		m_mousePositionProviders.push_back( _provider );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::removeMousePositionProvider( InputMousePositionProvider * _provider )
	{
        TVectorMousePositionProviders::iterator it_found = 
            std::find( m_mousePositionProviders.begin(), m_mousePositionProviders.end(), _provider );

        if( it_found == m_mousePositionProviders.end() )
        {
            LOGGER_ERROR(m_serviceProvider)("InputEngine::removeMousePositionProvider not found provider"
                );

            return;
        }

        m_mousePositionProviders.erase( it_found );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::onFocus( bool _focus )
	{
		std::fill( m_keyBuffer, m_keyBuffer + sizeof(m_keyBuffer), false );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::onKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		m_eventsAdd.push_back( ET_KEY );
		KeyEventParams params = { _point, _key, _char, _isDown };
		m_keyEventParams.push_back( params );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::onMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		m_eventsAdd.push_back( ET_MOUSEBUTTON );
		MouseButtonParams params = { _touchId, _point, _button, _isDown };
		m_mouseButtonEventParams.push_back( params );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::onMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell )
	{
		m_eventsAdd.push_back( ET_MOUSEMOVE );
		MouseMoveParams params = { _touchId, _point, _x, _y, _whell };
		m_mouseMoveEventParams.push_back( params );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::onMousePosition( unsigned int _touchId, const mt::vec2f & _point )
	{
		m_eventsAdd.push_back( ET_MOUSEPOSITION );
		MousePositionParams params = { _touchId, _point };
		m_mousePositionEventParams.push_back( params );
	}
    //////////////////////////////////////////////////////////////////////////
    void InputEngine::onMouseEnter( unsigned int _touchId, const mt::vec2f & _point )
    {
        m_eventsAdd.push_back( ET_MOUSEENTER );
        MousePositionParams params = { _touchId, _point };
        m_mousePositionEventParams.push_back( params );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputEngine::onMouseLeave( unsigned int _touchId, const mt::vec2f & _point )
    {
        m_eventsAdd.push_back( ET_MOUSELEAVE );
        MousePositionParams params = { _touchId, _point };
        m_mousePositionEventParams.push_back( params );
    }
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::keyEvent_( const KeyEventParams& _params )
	{
		m_keyBuffer[_params.key] = _params.isDown;

		mt::vec2f point;
		this->applyCursorPosition_( _params.point, point );

		APPLICATION_SERVICE(m_serviceProvider)
			->onKeyEvent( point, _params.key, _params.character, _params.isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::mouseButtonEvent_( const MouseButtonParams & _params )
	{
		m_mouseBuffer[ _params.button ] = _params.isDown;

		mt::vec2f point;
		this->applyCursorPosition_( _params.point, point );

		APPLICATION_SERVICE(m_serviceProvider)
			->onMouseButtonEvent( _params.touchId, point, _params.button, _params.isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::mouseMoveEvent_( const MouseMoveParams& _params )
	{
		mt::vec2f point;
		this->applyCursorPosition_( _params.point, point );

		APPLICATION_SERVICE(m_serviceProvider)
			->onMouseMove( _params.touchId, point, _params.x, _params.y, _params.whell );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::mousePositionEvent_( const MousePositionParams& _params )
	{
		mt::vec2f point;
		this->applyCursorPosition_( _params.point, point );

		APPLICATION_SERVICE(m_serviceProvider)
			->onMousePosition( _params.touchId, point );
	}
    //////////////////////////////////////////////////////////////////////////
    void InputEngine::mouseEnterEvent_( const MousePositionParams& _params )
    {
        mt::vec2f point;
        this->applyCursorPosition_( _params.point, point );

        APPLICATION_SERVICE(m_serviceProvider)
            ->onAppMouseEnter( point );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputEngine::mouseLeaveEvent_( const MousePositionParams& _params )
    {
        mt::vec2f point;
        this->applyCursorPosition_( _params.point, point );

        APPLICATION_SERVICE(m_serviceProvider)
            ->onAppMouseLeave();
    }
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution )
	{
        (void)_fullscreen;
        (void)_resolution;

		const Resolution & contentResolution = APPLICATION_SERVICE(m_serviceProvider)
            ->getContentResolution();

		const Viewport & renderViewport = APPLICATION_SERVICE(m_serviceProvider)
            ->getRenderViewport();

		size_t contentResolutionWidth = contentResolution.getWidth();
		size_t contentResolutionHeight = contentResolution.getHeight();
        
        float renderViewportWidth = renderViewport.getWidth();
        float renderViewportHeight = renderViewport.getHeight();

		m_dimentions.x = float(contentResolutionWidth);
		m_dimentions.y = float(contentResolutionHeight);

        float gameViewportAspect;
        Viewport gameViewport;

        APPLICATION_SERVICE(m_serviceProvider)
            ->getGameViewport( gameViewportAspect, gameViewport );

        m_inputViewport.begin = renderViewport.begin;
        m_inputViewport.end = renderViewport.end;

        m_inputOffset = gameViewport.begin;

        float gameViewportWidth = gameViewport.getWidth();
        float gameViewportHeight = gameViewport.getHeight();

        float width_scale = renderViewportWidth / gameViewportWidth;
        float height_scale = renderViewportHeight / gameViewportHeight;

        m_inputScale.x = 1.f / width_scale;
        m_inputScale.y = 1.f / height_scale;
	}
}
