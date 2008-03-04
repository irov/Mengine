#	pragma once

#	include "Interface/InputSystemInterface.h"
#	include "OIS/OIS.h"

class OgreInputSystem
	: public InputSystemInterface
	, public OIS::MouseListener
	, public OIS::KeyListener
{
public:
	OgreInputSystem();
	~OgreInputSystem();

public:
	bool initialize( WINDOW_HANDLE _winHandle ) override;
	bool captureMouse( int _x, int _y, int _maxX, int _maxY ) override;
	void releaseMouse() override;

public:
	void update() override;
	void destroy() override;

	void regHandle( InputSystemHandler * _handle ) override;

	//keyboard
	bool isKeyDown( int _key ) override;
	bool isModifierDown( int _modifier ) override;

	int getMouseX() const override;
	int getMouseY() const override;
	int getMouseWhell() const override;

	bool isAnyButtonDown() const override;
	bool isButtonDown( int _button ) const override;

protected:
	bool mouseMoved( const OIS::MouseEvent &arg );
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool keyPressed( const OIS::KeyEvent &arg );
	bool keyReleased( const OIS::KeyEvent &arg );

private:
	//OIS Input devices
	OIS::InputManager* m_inputManager;
	OIS::Mouse*    m_mouse;
	OIS::Keyboard* m_keyboard;

	InputSystemHandler * m_handler;
};