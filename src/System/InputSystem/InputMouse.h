#	pragma once

#	include "InputJoint.h"

class InputSystem;

class InputMouse
	: public InputJoint
{
public:
	InputMouse( InputSystem *_system );

public:
	HRESULT init() override;
	HRESULT restore() override;
	void update() override;

public:
	void setPosition( float _x, float _y, float _whell );
	void setSensitivity( float _sensitivity );
	void setRange( const float *_minRange, const float * _maxRange );

	const float * getPosition() const;
	const float * getDelta() const;

	//int GetCurPos		(int axis)const { return m_CurPos[axis]; };
	//int GetDeltaPos		(int axis)const { return m_DeltaPos[axis]; };

	bool isMove() const;
	bool isButtonDown(int _button) const;
	bool isButtonJustDown(int _button) const;
	bool isButtonJustUp(int _button) const;
	bool isAnyButtonDown() const;

private:
	DIDEVICEOBJECTDATA m_data;
	DIMOUSESTATE m_lastState;

	bool m_bMove;
	int m_button[8]; 

	float m_sensitivity;
	float m_position[3];
	float m_deltaPosition[3];
	float m_oldPosition[3];
	float m_range[6];
};
