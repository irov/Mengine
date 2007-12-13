#	pragma once

#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

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
	bool init( const char * _xmlFile, const char * _args ) override;
	void run() override;

	//void changeResolution( int _width, int _height, int _bits, bool _fullscreen );

protected:
	void createWindow( unsigned int _width, unsigned int _height, bool _fullscreen );

	bool frameStarted( const Ogre::FrameEvent & evt) override;
	bool frameEnded( const Ogre::FrameEvent & evt) override;

private:
	Ogre::Root * m_root;
	static Ogre::RenderWindow * m_renderWindow;
	static Menge::Application * m_application;
	std::string m_resourcePath;
	bool	m_running;
	float	m_frameTime;
	static HWND	m_hWnd;
	static bool	m_cursorInArea;
	static WINDOWINFO m_wndInfo;

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
