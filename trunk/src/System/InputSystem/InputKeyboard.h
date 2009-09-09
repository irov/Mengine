#	pragma once

#	include "InputJoint.h"

class InputSystemHandler;

class InputKeyboard
	: public InputJoint
{
public:
	InputKeyboard( InputSystem * _system );

public:
	HRESULT init() override;
	HRESULT restore() override;
	void update() override;

	void regHandle( InputSystemHandler * _handle );

public:
	bool isKey( int _key, int _state ) const;
	bool isAnyKeyDown() const;
	bool getChar( char *_char, int _state ) const;

private:
	unsigned char m_layerKeys;

	unsigned char m_keysBuffer[2][256];

	InputSystemHandler * m_handle;
};


