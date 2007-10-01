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

private:
	Ogre::Root *m_root;
	Ogre::String m_resourcePath;

	//Ogre::Camera *m_camera;
	//Ogre::SceneManager* m_sceneMgr;
	Ogre::RenderWindow* m_window;

	Menge::Application * m_application;

	typedef std::list<SystemDLL *> TListApplicationDLL;
	TListApplicationDLL m_listApplicationDLL;
};