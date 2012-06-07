#	pragma once

#	include "Interface/InputSystemInterface.h"

class OISInputSystem
	: public Menge::InputSystemInterface
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

private:
	//OIS Input devices
	bool m_mouseReleasing;

	Menge::InputSystemHandler * m_handler;
};
