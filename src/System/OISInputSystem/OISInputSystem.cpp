#	include "OgreInputSystem.h"
//#	include <Windows.h>
#	include <sstream>

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem(InputSystemInterface **_system)
{
	try
	{
		*_system = new OgreInputSystem();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem(InputSystemInterface *_system)
{
	delete static_cast<OgreInputSystem*>(_system);
}

OgreInputSystem::OgreInputSystem()
: m_inputManager(0)
, m_mouse(0)
, m_keyboard(0)
{
}
//////////////////////////////////////////////////////////////////////////
OgreInputSystem::~OgreInputSystem()
{
	destroy();
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::initialize( WINDOW_HANDLE _winHandle )
{
	std::ostringstream windowHndStr;
	windowHndStr << (int)_winHandle;
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
bool OgreInputSystem::captureMouse( float _x, float _y, float _maxX, float _maxY )
{
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
void OgreInputSystem::releaseMouse()
{
	m_mouse->setEventCallback( NULL );
	m_inputManager->destroyInputObject( m_mouse );
	m_mouse = NULL;
}
//////////////////////////////////////////////////////////////////////////
void OgreInputSystem::update()
{
	if( m_mouse )
	{
		m_mouse->capture();
	}
	m_keyboard->capture();
}
//////////////////////////////////////////////////////////////////////////
void OgreInputSystem::destroy()
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
void OgreInputSystem::regHandle( InputSystemHandler * _handle )
{
	m_handler = _handle;
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::isKeyDown( int _key )
{
	return m_keyboard->isKeyDown( OIS::KeyCode(_key) );
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::isModifierDown( int _modifier )
{
	return m_keyboard->isModifierDown( OIS::Keyboard::Modifier(_modifier) );
}
//////////////////////////////////////////////////////////////////////////
float OgreInputSystem::getMouseX() const
{
	const OIS::MouseState & ms = m_mouse->getMouseState();
	return float(ms.X.abs);
}
//////////////////////////////////////////////////////////////////////////
float OgreInputSystem::getMouseY() const
{
	const OIS::MouseState & ms = m_mouse->getMouseState();
	return float(ms.Y.abs);
}
//////////////////////////////////////////////////////////////////////////
int OgreInputSystem::getMouseWhell() const
{
	const OIS::MouseState & ms = m_mouse->getMouseState();
	return ms.Z.abs;
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::isAnyButtonDown() const
{
	const OIS::MouseState & ms = m_mouse->getMouseState();
	return ms.buttons > 0;
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::isButtonDown( int _button ) const
{
	const OIS::MouseState & ms = m_mouse->getMouseState();
	return ms.buttonDown( OIS::MouseButtonID(_button) );
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::mouseMoved( const OIS::MouseEvent &arg )
{
	if( m_handler )
	{
		m_handler->handleMouseMove( float(arg.state.X.rel), float(arg.state.Y.rel), arg.state.Z.rel );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if( m_handler )
	{
		m_handler->handleMouseButtonEvent( id, true );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if( m_handler )
	{
		m_handler->handleMouseButtonEvent( id, false );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::keyPressed( const OIS::KeyEvent &arg )
{
	if( m_handler )
	{
		m_handler->handleKeyEvent( arg.key, arg.text, true );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::keyReleased( const OIS::KeyEvent &arg )
{
	if( m_handler )
	{
		m_handler->handleKeyEvent( arg.key, arg.text, false );
	}

	return true;
}