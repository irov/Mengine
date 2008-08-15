#	include "Framework.h"

#	include "..\Menge\LogEngine.h"

#	include "..\Menge\Holder.h"
#	include "..\Menge\Player.h"
#	include "..\Menge\Scene.h"
#	include "..\Menge\Sprite.h"
#	include "..\Menge\RenderEngine.h"

#	include <fstream>

MengeSprite::MengeSprite(Menge::Sprite* _sprite)
: m_sprite(_sprite)
{

}

bool MengeSprite::activate()
{
	return m_sprite->activate();
}

void MengeSprite::setImageIndex( int _index )
{
	m_sprite->setImageIndex(_index);
}

int MengeSprite::getImageIndex()
{
	return m_sprite->getImageIndex();
}

void MengeSprite::setImageResource( char * _name )
{
	m_sprite->setImageResource(std::string(_name));
}

int MengeSprite::getImageCount()
{
	return m_sprite->getImageCount();
}

bool MengeSprite::getCenterAlign()
{
	return m_sprite->getCenterAlign();
}

void MengeSprite::flip( bool _x )
{
	m_sprite->flip(_x);
}

void MengeSprite::setAlpha( float _alpha )
{
	m_sprite->setAlpha(_alpha);
}

void MengeSprite::alphaTo( float _alpha, float _time )
{
	m_sprite->alphaTo(_alpha, _time);
}

void MengeSprite::setPos( float X, float Y )
{
	m_sprite->setLocalPosition(mt::vec2f(X, Y));
}


Menge::Sprite * MengeSprite::getSprite()
{
	return m_sprite;
}

Framework::Framework()
: platform(0)
, logSystem(0)
, profilerSystem(0)
, fileSystem(0)
, inputSystem(0)
, particleSystem(0)
, physicSystem2D(0)
, renderSystem(0)
, soundSystem(0)
{
}

MengeSprite * Framework::createSprite(char * name, float x, float y)
{
	Menge::Sprite * sprite = new Menge::Sprite();
	sprite->setName(std::string(name));
	sprite->setImageResource("default");
	sprite->setLocalPosition(mt::vec2f(x,y));
	//sprite->activate();

	MengeSprite * mengeSprite = new MengeSprite(sprite);
	return mengeSprite;
}

void Framework::layerAppend(char * _layer, MengeSprite * _sprite)
{
	Menge::Scene * scene = Menge::Holder<Menge::Player>::hostage()->getCurrentScene();


	//Menge::Sprite * sprite = new Menge::Sprite();
	//sprite->setImageResource("default");
	//sprite->setLocalPosition(mt::vec2f(200,200));
	//sprite->activate();

	Menge::Sprite * sprite = _sprite->getSprite();
	scene->layerAppend(std::string(_layer),sprite);
}

Framework::~Framework()
{
}

void Framework::setCurrentScene(char * currentScene)
{
	std::string sceneName(currentScene);
	Menge::Holder<Menge::Player>::hostage()->setCurrentScene(sceneName);
}

void Framework::onUpdate(float _timing)
{
	app->onUpdate(_timing);
}

void Framework::renderLine(unsigned int _color, float X, float Y, float DX, float DY)
{
	Menge::Holder<Menge::RenderEngine>::hostage()->renderLine(0xFF00FF00, mt::vec2f(X,Y), mt::vec2f(DX,DY));
}

void Framework::initMenge()
{
	const char * config_file = "application.xml";

	Menge::ApplicationInterface * platform = 0;

	if( initInterfaceSystem( &platform ) == false )
	{
		assert(0);
	}

	app = new Menge::Application( platform );

	Menge::LogSystemInterface * logSystem;
	initInterfaceSystem( &logSystem );
	app->setLogSystem( logSystem );

	Menge::ProfilerSystemInterface * profilerSystem;
	initInterfaceSystem( &profilerSystem );
	app->setProfilerSystem( profilerSystem );

	Menge::FileSystemInterface * fileSystem;
	initInterfaceSystem( &fileSystem );
	app->setFileSystem( fileSystem );

	fileSystem->loadPath(".");

	Menge::InputSystemInterface * inputSystem;
	initInterfaceSystem( &inputSystem );
	app->setInputSystem( inputSystem );

	Menge::ParticleSystemInterface * particleSystem;
	initInterfaceSystem( &particleSystem );
	app->setParticleSystem( particleSystem );

	Menge::PhysicSystem2DInterface * physicSystem2D;
	initInterfaceSystem( &physicSystem2D );
	app->setPhysicSystem2D( physicSystem2D );

	Menge::RenderSystemInterface * renderSystem;
	initInterfaceSystem( &renderSystem );
	app->setRenderSystem( renderSystem );

	Menge::SoundSystemInterface * soundSystem;
	initInterfaceSystem( &soundSystem );
	app->setSoundSystem( soundSystem );

	bool result = app->initialize( config_file, "" );	
}

void Framework::releaseMenge()
{
	releaseInterfaceSystem( platform );

	releaseInterfaceSystem( soundSystem );
	releaseInterfaceSystem( renderSystem );
	releaseInterfaceSystem( physicSystem2D );
	releaseInterfaceSystem( particleSystem );
	releaseInterfaceSystem( inputSystem );
	releaseInterfaceSystem( fileSystem );
	releaseInterfaceSystem( logSystem );
}

void Framework::createWindow(HWND _hwnd)
{
	bool result = app->loadGame();
	result = app->createRenderWindow(_hwnd);
	app->initGame(false);
}
/*
BOOL APIENTRY DllMain(
					  HMODULE,
					  DWORD ul_reason_for_call,
					  LPVOID)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}*/