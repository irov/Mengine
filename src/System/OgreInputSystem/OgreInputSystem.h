#	pragma once

#	include "System/OgreInputSystem/OgreInputSystemInterface.h"

class OgreInputSystem
	: public OgreInputSystemInterface
	, public OIS::MouseListener
	, public OIS::KeyListener
{
public:
	OgreInputSystem();
	~OgreInputSystem();

public:
	bool init( const OIS::ParamList & _params ) override;

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