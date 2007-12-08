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
	static Ogre::RenderWindow * m_renderWindow;
	Menge::Application * m_application;
	std::string m_resourcePath;
	HWND	m_hWnd;
	bool	m_running;
	float	m_frameTime;

	static LRESULT CALLBACK _wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

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

	void initParams();
};
