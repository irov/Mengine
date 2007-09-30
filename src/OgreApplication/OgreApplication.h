#	pragma once

#	include "Interface/ApplicationInterface.h"

#	include "Ogre.h"

#	include <list>

namespace Menge
{
	class Application;
}

class SystemDLL;

class OgreApplication
	: public ApplicationInterface
{
public:
	OgreApplication();
	~OgreApplication();

public:
	bool init( const char * _xmlFile ) override;
	void run() override;

public:
	bool createRoot(); // создаем начальную точки каркаса приложения
	void destroyRoot(); // очищаем все параметры каркаса приложения

	void setupResources(); // загружаем все ресурсы приложения

protected:
	//bool frameStarted(const Ogre::FrameEvent& evt);
	//bool frameEnded(const Ogre::FrameEvent& evt);

private:
	Ogre::Root *m_root;
	Ogre::String m_resourcePath;

	//Ogre::Camera *m_camera;
	//Ogre::SceneManager* m_sceneMgr;
	Ogre::RenderWindow* m_window;

	Application * m_application;

	typedef std::list<SystemDLL *> TListApplicationDLL;
	TListApplicationDLL m_listApplicationDLL;
};