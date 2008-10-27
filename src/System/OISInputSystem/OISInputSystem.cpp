#	include "OISInputSystem.h"
#	include <sstream>

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::InputSystemInterface **_system )
{
	try
	{
		*_system = new OISInputSystem();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::InputSystemInterface *_system )
{
	delete static_cast<OISInputSystem*>(_system);
}

OISInputSystem::OISInputSystem()
: m_inputManager(0)
, m_mouse(0)
, m_keyboard(0)
, m_mouseReleasing( false )
{
}
//////////////////////////////////////////////////////////////////////////
OISInputSystem::~OISInputSystem()
{
	destroy();
}
//////////////////////////////////////////////////////////////////////////
bool OISInputSystem::initialize( Menge::WindowHandle _winHandle )
{
	std::ostringstream windowHndStr;
	windowHndStr << (std::size_t)_winHandle;
	OIS::ParamList pl;
	pl.insert( std::make_pair(	"WINDOW", windowHndStr.str() ) );


	m_inputManager = OIS::InputManager::createInputSystem( pl );

	m_keyboard = static_cast<OIS::Keyboard*>( m_inputManager->createInputObject( OIS::OISKeyboard, true ) );
	
	//m_mouse->getMouseState().width = 1024;
	//m_mouse->getMouseState().height = 768;
	//m_mouse->setEventCallback( this );
	m_keyboard->setEventCallback( this );

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool OISInputSystem::captureMouse( float _x, float _y, float _maxX, float _maxY )
{
	if( m_mouseReleasing == true )
	{
		m_mouse->setEventCallback( NULL );
		m_inputManager->destroyInputObject( m_mouse );
		m_mouse = NULL;
		m_mouseReleasing = false;
	}
	m_mouse = static_cast<OIS::Mouse*>( m_inputManager->createInputObject( OIS::OISMouse, true ) );
	OIS::MouseState& state = const_cast<OIS::MouseState&>( m_mouse->getMouseState() );
	state.width = int(_maxX);
	state.height = int(_maxY);
	state.X.abs = int(_x);
	state.Y.abs = int(_y);
	m_mouse->setEventCallback( this );
	return true;
}
//////////////////////////////////////////////////////////////////////////
void OISInputSystem::releaseMouse()
{
	if( m_mouse != 0 )
	{
		m_mouseReleasing = true;
	}
}
//////////////////////////////////////////////////////////////////////////
void OISInputSystem::update()
{
	if( m_mouse )
	{
		m_mouse->capture();
	}
	if( m_mouseReleasing )
	{
		m_mouse->setEventCallback( NULL );
		m_inputManager->destroyInputObject( m_mouse );
		m_mouse = NULL;
		m_mouseReleasing = false;
	}
	m_keyboard->capture();
}
//////////////////////////////////////////////////////////////////////////
void OISInputSystem::destroy()
{
	if( m_inputManager )
	{
		if( m_mouse ) 
		{
			m_inputManager->destroyInputObject( m_mouse );
			m_mouse = 0;
		}
		if( m_keyboard )
		{
			m_inputManager->destroyInputObject( m_keyboard );
			m_keyboard = 0;
		}

		OIS::InputManager::destroyInputSystem( m_inputManager );
		m_inputManager = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
void OISInputSystem::regHandle( Menge::InputSystemHandler * _handle )
{
	m_handler = _handle;
}
//////////////////////////////////////////////////////////////////////////
bool OISInputSystem::isKeyDown( int _key )
{
	return m_keyboard->isKeyDown( OIS::KeyCode(_key) );
}
//////////////////////////////////////////////////////////////////////////
bool OISInputSystem::isModifierDown( int _modifier )
{
	return m_keyboard->isModifierDown( OIS::Keyboard::Modifier(_modifier) );
}
//////////////////////////////////////////////////////////////////////////
float OISInputSystem::getMouseX() const
{
	const OIS::MouseState & ms = m_mouse->getMouseState();
	return float(ms.X.abs);
}
//////////////////////////////////////////////////////////////////////////
float OISInputSystem::getMouseY() const
{
	const OIS::MouseState & ms = m_mouse->getMouseState();
	return float(ms.Y.abs);
}
//////////////////////////////////////////////////////////////////////////
int OISInputSystem::getMouseWhell() const
{
	const OIS::MouseState & ms = m_mouse->getMouseState();
	return ms.Z.abs;
}
//////////////////////////////////////////////////////////////////////////
bool OISInputSystem::isAnyButtonDown() const
{
	const OIS::MouseState & ms = m_mouse->getMouseState();
	return ms.buttons > 0;
}
//////////////////////////////////////////////////////////////////////////
bool OISInputSystem::isButtonDown( int _button ) const
{
	const OIS::MouseState & ms = m_mouse->getMouseState();
	return ms.buttonDown( OIS::MouseButtonID(_button) );
}
//////////////////////////////////////////////////////////////////////////
bool OISInputSystem::mouseMoved( const OIS::MouseEvent &arg )
{
	if( m_handler )
	{
		m_handler->handleMouseMove( float(arg.state.X.rel), float(arg.state.Y.rel), arg.state.Z.rel );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool OISInputSystem::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if( m_handler )
	{
		m_handler->handleMouseButtonEvent( id, true );
		m_handler->handleMouseButtonEventEnd( id, true );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool OISInputSystem::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if( m_handler )
	{
		m_handler->handleMouseButtonEvent( id, false );
		m_handler->handleMouseButtonEventEnd( id, false );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool OISInputSystem::keyPressed( const OIS::KeyEvent &arg )
{
	if( m_handler )
	{
		m_handler->handleKeyEvent( arg.key, arg.text, true );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool OISInputSystem::keyReleased( const OIS::KeyEvent &arg )
{
	if( m_handler )
	{
		m_handler->handleKeyEvent( arg.key, arg.text, false );
	}

	return true;
}