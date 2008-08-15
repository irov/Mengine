#	pragma once

using namespace System;

//#	include "ApplicationInterface.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/ProfilerSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/InputSystemInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/SoundSystemInterface.h"
#	include "Interface/ParticleSystemInterface.h"
#	include "Interface/PhysicSystemInterface.h"
#	include "Interface/PhysicSystem2DInterface.h"

#	include "..\Menge\Application.h"
#	include "..\Menge\RenderEngine.h"
//#	include "..\Menge\Sprite.h"

#	define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#	include <windows.h>

#define EXAMPLEUNMANAGEDDLL_API __declspec(dllexport)

namespace Menge
{
	class Sprite;
}

public class ref MengeSprite
{
public:

	MengeSprite(Menge::Sprite * _sprite);

public:
	bool activate();
public:
	void setImageIndex( int _index );
	int getImageIndex();
	void setImageResource( char * _name );
	int getImageCount();
	bool getCenterAlign();
	void flip( bool _x );
	void setAlpha( float _alpha );
	void alphaTo( float _alpha, float _time );
	Menge::Sprite * getSprite();
	void setPos( float X, float Y );
private:
	Menge::Sprite * m_sprite;
};

public class ref Framework
{
public:
	Framework();
	~Framework();
public:
	//void addResource();

	MengeSprite * createSprite(char * name, float x, float y);

	void setCurrentScene(char * currentScene);


	void layerAppend(char * _layer, MengeSprite * _sprite);

	void initMenge();
	void releaseMenge();

	void createWindow(HWND hwnd);


	void onUpdate(float _timing);

	void addNode();

	void renderLine(unsigned int _color, float X, float Y, float DX, float DY);
	
private: 
	Menge::Application * app;
	Menge::ApplicationInterface * platform;
	
	Menge::LogSystemInterface * logSystem;
	Menge::ProfilerSystemInterface * profilerSystem;
	Menge::FileSystemInterface * fileSystem;
	Menge::InputSystemInterface * inputSystem;
	Menge::ParticleSystemInterface * particleSystem;
	Menge::PhysicSystem2DInterface * physicSystem2D;
	Menge::RenderSystemInterface * renderSystem;
	Menge::SoundSystemInterface * soundSystem;
};