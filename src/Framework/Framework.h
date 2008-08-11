#	pragma once

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

#	define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#	include <windows.h>

#define EXAMPLEUNMANAGEDDLL_API __declspec(dllexport)

class EXAMPLEUNMANAGEDDLL_API Framework
{
public:
	Framework();
	~Framework();
public:
	//void addResource();

	void setCurrentScene();


	void createWindow(HWND hwnd);
	void step(float _timing);
private: 
	Menge::Application * app;
	Menge::ApplicationInterface * platform;
	Menge::RenderEngine* m_renderEngine;
	//Menge::LogEngine* m_logEngine;
};