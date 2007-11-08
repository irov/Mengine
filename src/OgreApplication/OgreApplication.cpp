#	include "OgreApplication.h"

#	include "Menge/Application.h"

#	include "System/OgreInputSystem/OgreInputSystemInterface.h"
#	include "System/OgreRenderSystem/OgreRenderSystemInterface.h"

#	include "System/OgreFileSystem/OgreFileSystemInterface.h"
#	include "Interface/SoundSystemInterface.h"

#	include "OIS/OIS.h"

#	include "SystemDLL.h"

#	include "XmlParser/XmlParser.h"

#	include <map>

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( ApplicationInterface** _ptrInterface )
{
	try
	{
		*_ptrInterface = new OgreApplication();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( ApplicationInterface* _ptrInterface )
{
	delete static_cast<OgreApplication*>(_ptrInterface);
}

//////////////////////////////////////////////////////////////////////////
OgreApplication::OgreApplication() 
: m_application(0)
, m_root(0)
, m_window(0)
{
	
}
//////////////////////////////////////////////////////////////////////////
OgreApplication::~OgreApplication()
{
	m_application->finalize();
	delete m_application;

	for each( SystemDLL * sysDll in m_listApplicationDLL )
	{
		delete sysDll;
	}

	if( m_window )
	{
		m_window->destroy();
		m_window = 0;
	}

	if( m_root ) 
	{				
		if( Ogre::RenderWindow * window = m_root->getAutoCreatedWindow() )
		{
			window->removeAllViewports();
		}

		delete m_root;
		m_root = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
bool OgreApplication::init( const char * _xmlFile )
{
	m_application = new Menge::Application;

#ifdef _DEBUG
	std::string DllModuleSetting = "DllModuleDebug";
	std::string DllFileSystem = "Systems/OgreFileSystem_d.dll";
	std::string DllRenderSystem = "Systems/OgreRenderSystem_d.dll";
	std::string DllInputSystem = "Systems/OgreInputSystem_d.dll";
	std::string DllSoundSystem = "Systems/SoundSystem_d.dll";
#else
	std::string DllModuleSetting = "DllModuleRelease";
	std::string DllFileSystem = "Systems/OgreFileSystem.dll";
	std::string DllRenderSystem = "Systems/OgreRenderSystem.dll";
	std::string DllInputSystem = "Systems/OgreInputSystem.dll";
	std::string DllSoundSystem = "Systems/SoundSystem.dll";
#endif

	printf("use file system [%s]\n", DllFileSystem.c_str() );

	SystemInterfaceDLL<OgreFileSystemInterface> * fileInterfaceDLL = 
		new SystemInterfaceDLL<OgreFileSystemInterface>(DllFileSystem);

	m_listApplicationDLL.push_back( fileInterfaceDLL );

	OgreFileSystemInterface * fileInterface = fileInterfaceDLL->getInterface();
	m_application->setFileSystem( fileInterface );

	printf("use input system [%s]\n", DllInputSystem.c_str() );

	SystemInterfaceDLL<OgreInputSystemInterface> * inputInterfaceDLL = 
		new SystemInterfaceDLL<OgreInputSystemInterface>(DllInputSystem);

	m_listApplicationDLL.push_back( inputInterfaceDLL );

	OgreInputSystemInterface * inputInterface = inputInterfaceDLL->getInterface();
	m_application->setInputSystem( inputInterface );

	printf("use render system [%s]\n", DllRenderSystem.c_str() );

	SystemInterfaceDLL<OgreRenderSystemInterface> * renderInterfaceDLL = 
		new SystemInterfaceDLL<OgreRenderSystemInterface>(DllRenderSystem);

	m_listApplicationDLL.push_back( renderInterfaceDLL );

	OgreRenderSystemInterface * renderInterface = renderInterfaceDLL->getInterface();
	m_application->setRenderSystem( renderInterface );

	printf("use sound system [%s]\n", DllSoundSystem.c_str() );

	SystemInterfaceDLL<SoundSystemInterface> * soundInterfaceDLL = 
		new SystemInterfaceDLL<SoundSystemInterface>(DllSoundSystem);

	m_listApplicationDLL.push_back( soundInterfaceDLL );

	SoundSystemInterface * soundInterface = soundInterfaceDLL->getInterface();
	m_application->setSoundSystem( soundInterface );

	std::string resourcePath = fileInterface->platformBundlePath();

#ifndef _DEBUG
	m_root = new Ogre::Root(
		resourcePath + "Plugins.cfg", 
		resourcePath + "Ogre.cfg", 
		resourcePath + "Ogre.log");
#else
	m_root = new Ogre::Root(
		resourcePath + "Plugins_d.cfg", 
		resourcePath + "Ogre.cfg", 
		resourcePath + "Ogre.log");
#endif

	// попробуем завестись на дефолтных
	if ( m_root->restoreConfig() == false ) 
	{ 			
		// ничего не получилось, покажем диалог
		if ( m_root->showConfigDialog() == false )
		{
			return false; 
		}
	}

	m_window = m_root->initialise( true );
	
	m_root->addFrameListener( this );

	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	m_window->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	inputInterface->init( pl );

	Ogre::ResourceGroupManager * resourceMgr = 
		Ogre::ResourceGroupManager::getSingletonPtr();

	fileInterface->init( resourceMgr );
	fileInterface->loadPath(".");

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	Ogre::ResourceGroupManager::getSingleton().openResource( "Game/Game.xml" );

	renderInterface->init( m_root, m_window );
	
	bool initialize = m_application->initialize( _xmlFile );

	return initialize;
}
//////////////////////////////////////////////////////////////////////////
bool OgreApplication::frameStarted( const Ogre::FrameEvent &evt)
{
	if( m_window->isActive() )
	{
		return m_application->update( size_t(evt.timeSinceLastFrame * 1000.f) );
	}

	return m_window->isClosed() == false;
}
//////////////////////////////////////////////////////////////////////////
bool OgreApplication::frameEnded( const Ogre::FrameEvent &evt)
{
	const Ogre::RenderTarget::FrameStats& stats = m_window->getStatistics();
	printf("fps = %f \n", stats.avgFPS);
	return true;
}
//////////////////////////////////////////////////////////////////////////
void OgreApplication::run()
{
	m_root->startRendering();
}
