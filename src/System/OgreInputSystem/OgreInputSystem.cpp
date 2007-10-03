#	include "OgreInputSystem.h"


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
bool OgreInputSystem::init( const OIS::ParamList & _params )
{
	OIS::ParamList local_params = _params;

	//local_params.insert( std::make_pair("WINDOW","0"));

	m_inputManager = OIS::InputManager::createInputSystem( local_params );

	m_keyboard = static_cast<OIS::Keyboard*>(m_inputManager->createInputObject( OIS::OISKeyboard, false ));
	m_mouse = static_cast<OIS::Mouse*>(m_inputManager->createInputObject( OIS::OISMouse, false ));

	m_mouse->getMouseState().width = 1024;
	m_mouse->getMouseState().height = 768;

	m_mouse->setEventCallback(this);
	m_keyboard->setEventCallback(this);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void OgreInputSystem::update()
{
	m_mouse->capture();
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
int OgreInputSystem::getMouseX() const
{
	const OIS::MouseState & ms = m_mouse->getMouseState();
	return ms.X.abs;
}
//////////////////////////////////////////////////////////////////////////
int OgreInputSystem::getMouseY() const
{
	const OIS::MouseState & ms = m_mouse->getMouseState();
	return ms.Y.abs;
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
		return m_handler->handleMouseMove( arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs );
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if( m_handler )
	{
		return m_handler->handleMouseButtonEvent( id, true );
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if( m_handler )
	{
		return m_handler->handleMouseButtonEvent( id, false );
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::keyPressed( const OIS::KeyEvent &arg )
{
	if( m_handler )
	{
		return m_handler->handleKeyEvent( arg.key, true );
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
bool OgreInputSystem::keyReleased( const OIS::KeyEvent &arg )
{
	if( m_handler )
	{
		return m_handler->handleKeyEvent( arg.key, false );
	}

	return false;
}