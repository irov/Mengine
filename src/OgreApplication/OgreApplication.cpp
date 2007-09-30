#	include "OgreApplication.h"

#	include "Menge/Application.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/SoundSystemInterface.h"
#	include "System/OgreInputSystem/OgreInputSystemInterface.h"
#	include "Interface/WinRenderSystemInterface.h"

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

// This function will locate the path to our OgreApplication on OS X,
// unlike windows you can not rely on the curent working directory
// for locating your configuration files and resources.

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE

#	include <CoreFoundation/CoreFoundation.h>
Ogre::String platformBundlePath( const Ogre::String & _path )
{
	char path[1024];
	CFBundleRef mainBundle = CFBundleGetMainBundle();    
	assert(mainBundle);

	CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);    
	assert(mainBundleURL);

	CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);    
	assert(cfStringRef);

	CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);
	CFRelease(mainBundleURL);
	CFRelease(cfStringRef);

	return Ogre::String(path) + '/' + _path;
}

#else

Ogre::String platformBundlePath( const Ogre::String & _path )
{
	return _path;
}

#endif

//////////////////////////////////////////////////////////////////////////
OgreApplication::OgreApplication() 
: m_application(0)
, m_root(0)
//, m_camera(0)
//, m_sceneMgr(0)
, m_window(0)
{
	m_resourcePath = platformBundlePath("");
} 
//////////////////////////////////////////////////////////////////////////
OgreApplication::~OgreApplication()
{
	delete m_application;

	for each( Menge::SystemDLL * sysDll in m_listApplicationDLL )
	{
		delete sysDll;
	}

	destroyRoot();
}
//////////////////////////////////////////////////////////////////////////
bool OgreApplication::init( const char * _xmlFile )
{
	m_application = new Menge::Application;

	std::string DllModuleSetting = "DllModule";
#ifdef _DEBUG
	DllModuleSetting += "Debug";
#else
	DllModuleSetting += "Release";
#endif

	typedef std::map<int, std::list<std::string>> TPackHierarchical;
	TPackHierarchical packHierarchical;

	std::string gameFile;

	FileSystemInterface * fileInterface = 0;
	OgreInputSystemInterface * inputInterface = 0;
	WinRenderSystemInterface * renderInterface = 0;
	SoundSystemInterface * soundInterface = 0;

	printf("parse application xml [%s] ...\n", _xmlFile );
	XML_PARSE_FILE( _xmlFile )
	{
		XML_CHECK_NODE_FOR_EACH("Application")
		{
			XML_CHECK_NODE_FOR_EACH( DllModuleSetting )
			{
				XML_DEF_ATTRIBUTES_NODE(DllFile);

				if( DllFile.empty() )
				{
					XML_CONTINUE();
				}

				XML_CHECK_NODE("FileSystem")
				{
					printf("use file system [%s]\n", DllFile.c_str() );

					Menge::SystemInterfaceDLL<FileSystemInterface> * interfaceDLL = 
						new Menge::SystemInterfaceDLL<FileSystemInterface>(DllFile);

					m_listApplicationDLL.push_back( interfaceDLL );

					fileInterface = interfaceDLL->getInterface();
					m_application->setFileSystem( fileInterface );
				}

				XML_CHECK_NODE("InputSystem")
				{
					printf("use input system [%s]\n", DllFile.c_str() );

					Menge::SystemInterfaceDLL<OgreInputSystemInterface> * interfaceDLL = 
						new Menge::SystemInterfaceDLL<OgreInputSystemInterface>(DllFile);

					m_listApplicationDLL.push_back( interfaceDLL );

					inputInterface = interfaceDLL->getInterface();
					m_application->setInputSystem( inputInterface );
				}

				XML_CHECK_NODE("RenderSystem")
				{
					printf("use render system [%s]\n", DllFile.c_str() );

					Menge::SystemInterfaceDLL<WinRenderSystemInterface> * interfaceDLL = 
						new Menge::SystemInterfaceDLL<WinRenderSystemInterface>(DllFile);

					m_listApplicationDLL.push_back( interfaceDLL );

					renderInterface = interfaceDLL->getInterface();
					m_application->setRenderSystem( renderInterface );
				}

				XML_CHECK_NODE("SoundSystem")
				{
					printf("use sound system [%s]\n", DllFile.c_str() );

					Menge::SystemInterfaceDLL<SoundSystemInterface> * interfaceDLL = 
						new Menge::SystemInterfaceDLL<SoundSystemInterface>(DllFile);

					m_listApplicationDLL.push_back( interfaceDLL );

					soundInterface = interfaceDLL->getInterface();
					m_application->setSoundSystem( soundInterface );
				}
			}

			XML_CHECK_NODE_FOR_EACH("Paks")
			{
				XML_CHECK_NODE("Pak")
				{
					std::string file;
					int priority;

					XML_VALUE_ATTRIBUTE("File",file);
					XML_VALUE_ATTRIBUTE("Priority",priority);

					packHierarchical[ priority ].push_back( file );
				}
			}

			//XML_CHECK_NODE_FOR_EACH("Display")
			//{
			//	XML_CHECK_VALUE_NODE("Width","Value",m_width);
			//	XML_CHECK_VALUE_NODE("Height","Value",m_height);
			//	XML_CHECK_VALUE_NODE("Bits","Value",m_bits);
			//	XML_CHECK_VALUE_NODE("FullScreen","Value",m_fullScreen);

			//	XML_CHECK_VALUE_NODE("Vsync","Value",m_vsync);
			//	XML_CHECK_VALUE_NODE("StencilBuffer","Value",m_stencilBuffer);
			//	XML_CHECK_VALUE_NODE("AntiAlias","Value",m_antiAlias);
			//	XML_CHECK_VALUE_NODE("PureSoftware","Value",m_pureSoftware);
			//}

			XML_CHECK_VALUE_NODE("Game", "File", gameFile );
		}
	}
	XML_INVALID_PARSE()
	{
		//TODO: ERROR
		printf("parse application xml failed\n");
		return false;
	}

	if( createRoot() == false )
	{
		return false;
	}

	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	m_window->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	inputInterface->init( pl );

	m_application->init();

	for each( const TPackHierarchical::value_type & it in packHierarchical )
	{
		for each( const std::string & pak in it.second )
		{
			m_application->loadPak( pak, it.first );
		}
	}

	if( m_application->createGame( gameFile ) == false )
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void OgreApplication::run()
{
	//m_root->startRendering();
}
//////////////////////////////////////////////////////////////////////////
bool OgreApplication::createRoot() // создаем начальную точки каркаса приложения
{
#ifndef _DEBUG
	m_root = new Ogre::Root(
		m_resourcePath + "Plugins.cfg", 
		m_resourcePath + "Ogre.cfg", 
		m_resourcePath + "Ogre.log");
#else
	m_root = new Ogre::Root(
		m_resourcePath + "Plugins_d.cfg", 
		m_resourcePath + "Ogre.cfg", 
		m_resourcePath + "Ogre.log");
#endif
	setupResources();

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

	//m_sceneMgr = m_root->createSceneManager( Ogre::ST_GENERIC, "defaultSceneManager" );

	//m_camera = m_sceneMgr->createCamera("defaultCamera");
	//m_camera->setNearClipDistance(5);

	// Create one viewport, entire window
	//Ogre::Viewport* vp = m_window->addViewport( m_camera );
	// Alter the camera aspect ratio to match the viewport

	//float actualWidth = vp->getActualWidth();
	//float actualHeight = vp->getActualWidth();
	//float actualAspect = actualWidth / actualHeight;

	//m_camera->setAspectRatio( actualAspect );

	// Set default mipmap level (NB some APIs ignore this)
	//Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Load resources
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	//m_root->addFrameListener(this);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void OgreApplication::destroyRoot() // очищаем все параметры каркаса приложения
{
	// очищаем сцену
	//if( m_sceneMgr )
	//{
	//	m_sceneMgr->clearScene();
	//	m_sceneMgr->destroyAllCameras();
	//	m_sceneMgr = 0;
	//}

	if( m_window )
	{
		m_window->destroy();
		m_window = 0;
	}
	if( m_root ) 
	{
		m_root->getAutoCreatedWindow()->removeAllViewports();
		delete m_root;
		m_root = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreApplication::setupResources() // загружаем все ресурсы приложения
{
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load( m_resourcePath + "resources.cfg" );

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while( seci.hasMoreElements() )
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;

			Ogre::ResourceGroupManager::getSingletonPtr()
				->addResourceLocation( platformBundlePath(archName), typeName, secName );
		}
	}
}
////////////////////////////////////////////////////////////////////////////
//bool OgreApplication::frameStarted( const Ogre::FrameEvent& evt )
//{
//	return true;
//}
////////////////////////////////////////////////////////////////////////////
//bool OgreApplication::frameEnded( const Ogre::FrameEvent& evt )
//{
//	return true;
//};