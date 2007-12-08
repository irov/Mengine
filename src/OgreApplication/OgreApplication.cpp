#	include "OgreApplication.h"

#	include "Menge/Application.h"

#	include "System/OgreInputSystem/OgreInputSystemInterface.h"
#	include "System/OgreRenderSystem/OgreRenderSystemInterface.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/SoundSystemInterface.h"
#	include "Interface/ParticleSystemInterface.h"
#	include "Interface/FileSystemInterface.h"


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
{}
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
		Ogre::RenderWindow * window = m_root->getAutoCreatedWindow();

		if( window != NULL )
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
	m_application = new Menge::Application();

#ifdef _DEBUG
	std::string DllModuleSetting = "DllModuleDebug";
	std::string DllLogSystem = "Systems_d/OgreLogSystem_d.dll";
	std::string DllFileSystem = "Systems_d/OgreFileSystem_d.dll";
	std::string DllRenderSystem = "Systems_d/OgreRenderSystem_d.dll";
	std::string DllInputSystem = "Systems_d/OgreInputSystem_d.dll";
	std::string DllSoundSystem = "Systems_d/SoundSystem_d.dll";
	std::string DllParticleSystem = "Systems_d/AstralaxParticleSystem_d.dll";
#else
	std::string DllModuleSetting = "DllModuleRelease";
	std::string DllLogSystem = "Systems/OgreLogSystem.dll";
	std::string DllFileSystem = "Systems/OgreFileSystem.dll";
	std::string DllRenderSystem = "Systems/OgreRenderSystem.dll";
	std::string DllInputSystem = "Systems/OgreInputSystem.dll";
	std::string DllSoundSystem = "Systems/SoundSystem.dll";
	std::string DllParticleSystem = "Systems/AstralaxParticleSystem.dll";
#endif

	printf("use log system [%s]\n", DllLogSystem.c_str() );

	LogSystemInterface * logInterface
		= addSystem<LogSystemInterface>( DllLogSystem );

	m_application->setLogSystem( logInterface );

	printf("use file system [%s]\n", DllFileSystem.c_str() );

	FileSystemInterface * fileInterface
		= addSystem<FileSystemInterface>( DllFileSystem );

	m_application->setFileSystem( fileInterface );

	printf("use input system [%s]\n", DllInputSystem.c_str() );

	OgreInputSystemInterface * inputInterface
		= addSystem<OgreInputSystemInterface>( DllInputSystem );

	m_application->setInputSystem( inputInterface );

	printf("use render system [%s]\n", DllRenderSystem.c_str() );

	OgreRenderSystemInterface * renderInterface
		= addSystem<OgreRenderSystemInterface>( DllRenderSystem );

	m_application->setRenderSystem( renderInterface );

	printf("use sound system [%s]\n", DllSoundSystem.c_str() );

	SoundSystemInterface * soundInterface
		= addSystem<SoundSystemInterface>( DllSoundSystem );

	m_application->setSoundSystem( soundInterface );

	printf("use particle system [%s]\n", DllParticleSystem.c_str() );

	ParticleSystemInterface * particleInterface
		= addSystem<ParticleSystemInterface>( DllParticleSystem );

	m_application->setParticleSystem( particleInterface );

	m_resourcePath = fileInterface->platformBundlePath();

	m_root = new Ogre::Root(
		"",
		"",
		m_resourcePath + "Menge.log");

	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	fileInterface->loadPath(".");

	bool initialize = m_application->initialize( _xmlFile );

	initParams();

	m_window->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;

	OIS::ParamList pl;

	pl.insert( std::make_pair(	"WINDOW", windowHndStr.str() ) );

	inputInterface->init( pl );
	renderInterface->init( m_root, m_window );

	m_application->createGame();

	return initialize;
}
//////////////////////////////////////////////////////////////////////////
void OgreApplication::initParams()
{
	int bits = m_application->getScreenBits();
	int screenWidth = m_application->getScreenWidth();
	int screenHeight = m_application->getScreenHeight();
	bool fullscreen = m_application->isFullScreen();
	std::string renderDriver = m_application->getRenderDriver();

	m_root->loadPlugin( m_resourcePath + renderDriver );

	Ogre::RenderSystem * renderSystem = m_root->getAvailableRenderers()->at( 0 );
	m_root->setRenderSystem( renderSystem );

	m_root->initialise( false );

	m_root->addFrameListener( this );

	Ogre::NameValuePairList params;
	params.insert( std::make_pair("Colour Depth", Ogre::StringConverter::toString( bits ) ) );

	m_window = m_root->createRenderWindow( "Menge-engine", screenWidth, screenHeight, fullscreen, &params );
}
//////////////////////////////////////////////////////////////////////////
bool OgreApplication::frameStarted( const Ogre::FrameEvent &evt)
{
	if( m_window->isActive() )
	{
		return m_application->update( size_t( evt.timeSinceLastFrame * 1000.f ) );
	}

	return m_window->isClosed() == false;
}
//////////////////////////////////////////////////////////////////////////
bool OgreApplication::frameEnded( const Ogre::FrameEvent &evt)
{
	const Ogre::RenderTarget::FrameStats& stats = m_window->getStatistics();
	//printf("fps = %f \n", stats.avgFPS);
	//m_application->frameEnded();
	return true;
}
//////////////////////////////////////////////////////////////////////////
void OgreApplication::run()
{
	m_root->startRendering();
}
//////////////////////////////////////////////////////////////////////////