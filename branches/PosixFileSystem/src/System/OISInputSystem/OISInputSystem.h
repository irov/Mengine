#	pragma once

#	include "Interface/InputSystemInterface.h"
#	include "OIS.h"

class OISInputSystem
	: public Menge::InputSystemInterface
	, public OIS::MouseListener
	, public OIS::KeyListener
{
public:
	OISInputSystem();
	~OISInputSystem();

public:
	bool initialize( Menge::WindowHandle _winHandle ) override;
	bool captureMouse( float _x, float _y, float _maxX, float _maxY ) override;
	void releaseMouse() override;

public:
	void update() override;
	void destroy() override;

	void regHandle( Menge::InputSystemHandler * _handle ) override;

	//keyboard
	bool isKeyDown( int _key ) override;
	bool isModifierDown( int _modifier ) override;

	float getMouseX() const override;
	float getMouseY() const override;
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
	bool m_mouseReleasing;

	Menge::InputSystemHandler * m_handler;
};
