# pragma once

using namespace System;

// Forward declaration
class MengeSprite;

public ref class ManagedSprite
{
public:
	ManagedSprite(MengeSprite * _mengeSprite);
	bool activate();
public:
	void setImageIndex( int _index );
	int getImageIndex();
	void setImageResource( String^ _name );
	int getImageCount();
	bool getCenterAlign();
	void flip( bool _x );
	void setAlpha( float _alpha );
	void alphaTo( float _alpha, float _time );
	MengeSprite * getMengeSprite();

	void setPos( float X, float Y );
private:
MengeSprite * m_pUnmanagedTestClass;
};

class Framework;

public ref class ManagedFramework
{
public:
	ManagedFramework();
	virtual ~ManagedFramework();
	
	void initMenge();
	void releaseMenge();

	void createWindow(IntPtr hWnd);
	void setCurrentScene(String^ strValue);
	ManagedSprite ^ createSprite(String^ name, float x, float y);

	// app properties
	void onUpdate(float _timing);


	void layerAppend(String^ _layer, ManagedSprite ^ _sprite);

	void renderLine(unsigned int _color, float X, float Y, float DX, float DY);
private:
	Framework * m_pUnmanagedTestClass;
};