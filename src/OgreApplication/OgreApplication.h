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
	, public Ogre::FrameListener
{
public:
	OgreApplication();
	~OgreApplication();

public:
	bool init( const char * _xmlFile ) override;
	void run() override;

protected:
	void createWindow( unsigned int _width, unsigned int _height, bool _fullscreen );

	bool frameStarted( const Ogre::FrameEvent & evt) override;
	bool frameEnded( const Ogre::FrameEvent & evt) override;

private:
	Ogre::Root * m_root;
	Ogre::RenderWindow * m_window;
	Menge::Application * m_application;

	typedef std::list<SystemDLL *> TListApplicationDLL;
	TListApplicationDLL m_listApplicationDLL;

	template<class T>
	T * addSystem( const std::string & _file )
	{
		SystemInterfaceDLL<T> * InterfaceDLL = 
			new SystemInterfaceDLL<T>( _file );

		m_listApplicationDLL.push_back( InterfaceDLL  );

		T * interface_ = InterfaceDLL ->getInterface();

		return interface_;
	}
};